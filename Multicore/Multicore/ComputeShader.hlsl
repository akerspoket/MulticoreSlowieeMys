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
	bool result;
	float3 direction = {0,0,1};
	direction = 1.0f / direction.xyz;
	float2 screenPos = threadID.xy;
	//float t1 = (vertexinput[0][0] - screenPos.x) * direction.x;
	//float t2 = (vertexinput[0][2] - screenPos.x) * direction.x;

	//float t3 = (vertexinput[0][1] - screenPos.y) * direction.y;
	//float t4 = (vertexinput[0][3] - screenPos.y) * direction.y;
	//
	output[threadID.xy] = float4(float3(1, 0, 1) * (1 - length(threadID.xy - float2(400, 400)) / 400.0f), 1);
}