#include "Common.hlsl"

struct VertexIn
{
	float3 position : POSITION;
};

float4 VS(VertexIn input, instanceID : SV_InstanceID) SV_POSITION
{
	return mul(mul(mul(
		float4(input.position, 1.0f),
		gGameObjectInfos[instanceID].m_mtxWorld),
		gmtxView),
		gmtxProjection);
}