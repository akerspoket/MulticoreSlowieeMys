struct VertexInputType
{
	float4 position;
	float2 texCoord;
};

RWTexture2D<float4> output : register(u0);
RWStructuredBuffer<VertexInputType> vertexinput: register(u1);

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	output[threadID.xy] = float4(float3(1, 0, 1) * (1 - length(threadID.xy - float2(400, 400)) / 400.0f), 1);
}