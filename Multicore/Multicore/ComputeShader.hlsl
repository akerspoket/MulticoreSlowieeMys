struct VertexInputType
{
	float3 position;
	float2 texCoord;
};

struct Ray
{
	float3 Origin;
	float3 Direction;
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
#define N 800
#define EPSILON 0.00000001

float TriangleIntersection(float3 V1, float3 V2, float3 V3, float3 RayOrigin, float3 RayDirection);
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

	//ray.Direction = 1.0f / ray.Direction.xyz;

	//Hit detection vs AABB
	//float t1 = (vertexinput[0].position.x - (((float)threadID.x/800.0f)-0.5f)*2.0f) * ray.Direction.x;
	//float t2 = (vertexinput[2].position.x - (((float)threadID.x / 800.0f) - 0.5f)*2.0f) * ray.Direction.x;
	//float tmin = min(t1, t2);
	//float tmax = max(t1, t2);

	//t1 = (vertexinput[1].position.y - (((float)threadID.y / 800.0f) - 0.5f)*2.0f) * ray.Direction.y;
	//t2 = (vertexinput[3].position.y - (((float)threadID.y / 800.0f) - 0.5f)*2.0f) * ray.Direction.y;
	//tmin = max(tmin, min(t1, t2));
	//tmax = min(tmax, max(t1, t2));

	//t1 = (vertexinput[1].position.z - (((float)threadID.z / 800.0f) - 0.5f)*2.0f) * ray.Direction.z;
	//t2 = (vertexinput[3].position.z - (((float)threadID.z / 800.0f) - 0.5f)*2.0f) * ray.Direction.z;
	//tmin = max(tmin, min(t1, t2));
	//tmax = min(tmax, max(t1, t2));

	//if (tmax < 0 || tmin > tmax)
	//{
	//	output[threadID.xy] = float4(1,0,0, 1);
	//}
	//else
	//{
	//	output[threadID.xy] = float4((float)threadID.x/800, (float)threadID.y / 800, 0, 1);
	//}

	//Hit detection vs Triangle
	uint length;
	uint stride;
	indexinput.GetDimensions(length, stride);
	for (int i = 0; i < length; i+=3)
	{
		float t = TriangleIntersection(vertexinput[indexinput[i]].position, vertexinput[indexinput[i+1]].position, vertexinput[indexinput[i+2]].position, ray.Origin, ray.Direction);
		if (t > 0) //z should maybe be 0, dunno
		{
			output[threadID.xy] = float4((float)threadID.x / 800, (float)threadID.y / 800, 0, 1);
			return;
		}
	}


}

float TriangleIntersection(float3 V1, float3 V2, float3 V3, float3 RayOrigin, float3 RayDirection)
{
	float3 e1, e2; //Edge1, Edge2
	float3 P, Q, T;
	float det, inv_det, u, v;
	float t; //distance?

	e1 = V2 - V1;
	e2 = V3 - V1;
	P = cross(RayDirection, e2);
	det = dot(e1, P);
	if (det > -EPSILON && det < EPSILON)
	{
		return -1;
	}
	inv_det = 1.0f / det;

	T = RayOrigin - V1;
	u = dot(T, P) * inv_det;
	if (u < 0.0f || u > 1.0f)
	{
		return -1;
	}

	Q = cross(T, e1);
	v = dot(RayDirection, Q) * inv_det;
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