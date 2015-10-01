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
	float2 TexCoord;
	float HitDistance;
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
#define N 800
#define EPSILON 0.00000001

float TriangleIntersection(float3 V1, float3 V2, float3 V3, Ray ray);
float SphereIntersection(SpherePrimitive sphere, Ray ray);
void SetRayTexCoord(float3 P1, float3 P2, float3 P3, float2 T1, float2 T2, float2 T3, inout Ray ray);
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


	//Hit detection vs Triangle
	uint length;
	uint stride;
	indexinput.GetDimensions(length, stride);
	float sphereDist = SphereIntersection(sphereinput[0],  ray);
	if (sphereDist > 0)
	{
		ray.HitDistance = sphereDist;
		ray.TexCoord = float2(1.0f, 0.0f);
	}
	for (int i = 0; i < length; i+=3)
	{
		float t = TriangleIntersection(vertexinput[indexinput[i]].position, vertexinput[indexinput[i+1]].position, vertexinput[indexinput[i+2]].position, ray);
		if (t > 0 && t < ray.HitDistance) //z should maybe be 0, dunno
		{
			ray.HitDistance = t;
			ray.TexCoord = float2(0.0f, 1.0f);
		}
	}
	output[threadID.xy] = float4(ray.TexCoord, 0.0f, 1.0f);

}

float TriangleIntersection(float3 V1, float3 V2, float3 V3, Ray ray)
{
	float3 e1, e2; //Edge1, Edge2
	float3 P, Q, T;
	float det, inv_det, u, v;
	float t; //distance?

	e1 = V2 - V1;
	e2 = V3 - V1;
	P = cross(ray.Direction, e2);
	det = dot(e1, P);
	if (det > -EPSILON && det < EPSILON)
	{
		return -1;
	}
	inv_det = 1.0f / det;

	T = ray.Origin - V1;
	u = dot(T, P) * inv_det;
	if (u < 0.0f || u > 1.0f)
	{
		return -1;
	}

	Q = cross(T, e1);
	v = dot(ray.Direction, Q) * inv_det;
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

float SphereIntersection(SpherePrimitive sphere, Ray ray)
{
	float b = dot(ray.Direction, ray.Origin - sphere.Origin);
	float c = dot(ray.Origin - sphere.Origin, ray.Origin - sphere.Origin) - pow(sphere.Radius, 2.0f);
	bool hit;
	float f = pow(b, 2.0f) - c;
	hit = f >= 0.0f ? true : false;
	if (hit)
	{
		return -b - sqrt(f);
	}

	return -1;

}

void SetRayTexCoord(float3 P1, float3 P2, float3 P3, float2 T1, float2 T2, float2 T3, inout Ray ray)
{
	//Own calculation, might not work		//Här finns det plats för optimization, gör vectorer å normalisera istället för att bli av med totdist
	float3 HitPoint = ray.Origin + ray.HitDistance * ray.Direction;
	float dist1 = length(HitPoint - P1);
	float dist2 = length(HitPoint - P2);
	float dist3 = length(HitPoint - P3);	//Beräknar längde från träffpunkten till varje vertice
	float totDist = dist1 + dist2 + dist3;
	dist1 /= totDist;
	dist2 /= totDist;
	dist3 /= totDist;	//Delar med totDist för att få fram % av hur mycket av varje texcoord vi ska ta
	ray.TexCoord = dist1 * T1 + dist2*T2 + dist3 * T3; //Beräknar texcoord utifrån hur långt bort ifrån varje  åunkt vi ligger
}