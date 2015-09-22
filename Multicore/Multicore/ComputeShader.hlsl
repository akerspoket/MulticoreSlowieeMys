struct VertexInputType
{
	float3 position;
	float2 texCoord;
};

cbuffer Matrices : register(b0)
{
	float4x4 WVP;
};

RWTexture2D<float4> output : register(u0);
//RWStructuredBuffer<VertexInputType> vertexinput: register(u1);
StructuredBuffer<VertexInputType> vertexinput: register(t0);


[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	float3 direction = {0.0f,0.0f,1.0f};
	direction = 1.0f / direction.xyz;
	VertexInputType tFixedPos[4];
	tFixedPos[0].position = mul(vertexinput[0].position, WVP);
	tFixedPos[1].position = mul(vertexinput[1].position, WVP);
	tFixedPos[2].position = mul(vertexinput[2].position, WVP);
	tFixedPos[3].position = mul(vertexinput[3].position, WVP);
	float t1 = (tFixedPos[0].position.x - (((float)threadID.x/800.0f)-0.5f)*2.0f) * direction.x;
	float t2 = (tFixedPos[2].position.x - (((float)threadID.x / 800.0f) - 0.5f)*2.0f) * direction.x;
	float tmin = min(t1, t2);
	float tmax = max(t1, t2);

	t1 = (tFixedPos[1].position.y - (((float)threadID.y / 800.0f) - 0.5f)*2.0f) * direction.y;
	t2 = (tFixedPos[3].position.y - (((float)threadID.y / 800.0f) - 0.5f)*2.0f) * direction.y;
	tmin = max(tmin, min(t1, t2));
	tmax = min(tmax, max(t1, t2));

	t1 = (tFixedPos[1].position.z - (((float)threadID.z / 800.0f) - 0.5f)*2.0f) * direction.z;
	t2 = (tFixedPos[3].position.z - (((float)threadID.z / 800.0f) - 0.5f)*2.0f) * direction.z;
	tmin = max(tmin, min(t1, t2));
	tmax = min(tmax, max(t1, t2));

	if (tmax < 0 || tmin > tmax)
	{
		output[threadID.xy] = float4(1,0,0, 1);
	}
	else
	{
		output[threadID.xy] = float4((float)threadID.x/800, (float)threadID.y / 800, 0, 1);
	}
}