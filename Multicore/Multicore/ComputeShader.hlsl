struct VertexInputType
{
	float3 position;
	float2 texCoord;
	float3 normal;
};

struct SpherePrimitive
{
	float3 Origin;
	float Radius;
};

struct Ray
{
	float3 Origin;
	float3 Direction;
	float3 HitPoint;
	float2 TexCoord;
	float HitDistance;
	int PrimitiveIndex;
};

cbuffer Matrices : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
};

RWTexture2D<float4> output : register(u0);
//RWStructuredBuffer<VertexInputType> vertexinput: register(u1);
StructuredBuffer<VertexInputType> vertexinput: register(t0);
StructuredBuffer<uint> indexinput: register(t1);
StructuredBuffer<SpherePrimitive> sphereinput: register(t2);
StructuredBuffer<float3> pointlight : register(t3);
Texture2D ObjTexture : register(t4);
SamplerState ObjSamplerState : register(s0);
#define N 800
#define EPSILON 0.00001

float TriangleIntersection(float3 V1, float3 V2, float3 V3, float3 Direction, float3 Origin);
float SphereIntersection(SpherePrimitive sphere, float3 Direction, float3 Origin);
void SetRayTexCoord(float3 P1, float3 P2, float3 P3, float2 T1, float2 T2, float2 T3, inout Ray ray);
float CalculateLight(float3 NewRayOrigin, int NumberOfPrimitives, float3 Normal); //new ray origin = hitpoint
[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	//Ray creation
	float4x4 WVPMatrix = ViewMatrix;//mul(WorldMatrix, mul(ViewMatrix, ProjectionMatrix)); 
	float inverse = 1.0f / (float(N));
	float y = -float(2.0f * threadID.y +1.0f  - N) * inverse; //2*ty/N + 1/N -1
	float x = float(2.0f * threadID.x +1.0f - N) * inverse;
	float z = 2.0f;

	Ray ray;
	float4 aux = (mul(float4(0, 0, 0, 1.0f), WVPMatrix));
	ray.Origin = aux.xyz / aux.w; //Blev rätt, på kamerans position
	float3 pixelPosition = mul(float4(x, y, z, 1.0f), WVPMatrix).xyz;
	ray.Direction = normalize(pixelPosition - ray.Origin);
	ray.HitDistance = 10000000;

	ray.TexCoord = float2(1.0f, 1.0f);


	//Hit detection vs Triangle & Sphere
	uint length;
	uint stride;
	indexinput.GetDimensions(length, stride);
	float sphereDist = SphereIntersection(sphereinput[0],  ray.Direction, ray.Origin);
	if (sphereDist > 0)
	{
		ray.HitDistance = sphereDist;
		ray.HitPoint = ray.Origin + ray.HitDistance * ray.Direction;
		ray.PrimitiveIndex = -1;
		ray.TexCoord = float2(1.0f, 0.0f);
	}
	for (int i = 0; i < length; i+=3)
	{
		float t = TriangleIntersection(vertexinput[indexinput[i]].position, vertexinput[indexinput[i+1]].position, vertexinput[indexinput[i+2]].position, ray.Direction, ray.Origin);
		if (t > 0 && t < ray.HitDistance) //z should maybe be 0, dunno
		{	
			ray.HitDistance = t;	//If better hit calculate texcoords
			ray.HitPoint = ray.Origin + ray.HitDistance * ray.Direction;
			ray.PrimitiveIndex = i;
			SetRayTexCoord(vertexinput[indexinput[i]].position, vertexinput[indexinput[i + 1]].position, vertexinput[indexinput[i + 2]].position,
				vertexinput[indexinput[i]].texCoord, vertexinput[indexinput[i + 1]].texCoord, vertexinput[indexinput[i + 2]].texCoord,ray);
			//And calculate light
		}
	}
	output[threadID.xy] = float4(ray.TexCoord, 0.0f, 1.0f);
	if (ray.HitDistance < 10000000) //If a hit was detected sample from texture
	{
		float illumination = CalculateLight(ray.HitPoint, length, vertexinput[indexinput[i]].normal);
		output[threadID.xy] = ObjTexture.SampleLevel(ObjSamplerState, ray.TexCoord, 0) * illumination;
	}
	

}

float TriangleIntersection(float3 V1, float3 V2, float3 V3, float3 Direction, float3 Origin)
{
	float3 e1, e2; //Edge1, Edge2
	float3 P, Q, T;
	float det, inv_det, u, v;
	float t; //distance?

	e1 = V2 - V1;
	e2 = V3 - V1;
	P = cross(Direction, e2);
	det = dot(e1, P);
	if (det > -EPSILON && det < EPSILON)
	{
		return -1;
	}
	inv_det = 1.0f / det;

	T = Origin - V1;
	u = dot(T, P) * inv_det;
	if (u < 0.0f || u > 1.0f)
	{
		return -1;
	}

	Q = cross(T, e1);
	v = dot(Direction, Q) * inv_det;
	if (v < 0.0f || v + u > 1.0f)
	{
		return -1;
	}

	t = dot(e2, Q) * inv_det;

	if (t > EPSILON)
	{
		return t;
	}

	return -1;
}

float SphereIntersection(SpherePrimitive sphere, float3 Direction, float3 Origin)
{
	float b = dot(Direction, Origin - sphere.Origin);
	float c = dot(Origin - sphere.Origin, Origin - sphere.Origin) - pow(sphere.Radius, 2.0f);
	bool hit;
	float f = pow(b, 2.0f) - c;
	hit = f >= 0.0f ? true : false;
	if (hit)
	{
		float t = -b - sqrt(f);
		if (t > EPSILON)
		{
			return t;
		}
		
	}

	return -1;

}

void SetRayTexCoord(float3 P1, float3 P2, float3 P3, float2 T1, float2 T2, float2 T3, inout Ray ray)
{

	float v, w, u;
	float3 v0 = P2 - P1, v1 = P3 - P1, v2 = ray.HitPoint - P1;
	float d00 = dot(v0, v0);
	float d01 = dot(v0, v1);
	float d11 = dot(v1, v1);
	float d20 = dot(v2, v0);
	float d21 = dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	v = (d11 * d20 - d01 * d21) / denom;
	w = (d00 * d21 - d01 * d20) / denom;
	u = 1.0f - v - w;
	ray.TexCoord = u * T1 + v *T2 + w * T3; //Beräknar texcoord utifrån hur långt bort ifrån varje  åunkt vi ligger
}

float CalculateLight(float3 NewRayOrigin, int NumberOfPrimitives, float3 Normal)
{
	float3 NewRayDirection = pointlight[0] - NewRayOrigin;
	float DistanceToLight = length(NewRayDirection);
	NewRayDirection = normalize(NewRayDirection);
	float hitDistance = SphereIntersection(sphereinput[0], NewRayDirection, NewRayOrigin);
	float3 normal;
	if (hitDistance < DistanceToLight && hitDistance > 0)
	{
		return 0.5;
	}
	hitDistance = DistanceToLight + 2;
	for (int i = 0; i < NumberOfPrimitives; i++)
	{
		float t = TriangleIntersection(vertexinput[indexinput[i]].position, vertexinput[indexinput[i + 1]].position, vertexinput[indexinput[i + 2]].position, NewRayDirection, NewRayOrigin);
		if (t > 0 && t < hitDistance) //z should maybe be 0, dunno
		{
			hitDistance = t; 
			if (hitDistance < DistanceToLight)
			{
				return 0.5;
			}
		}
	}

	return 1; // dot(Normal, NewRayDirection); //doesnt work proparly
	//Dont know if we need distance between light and hit object
}