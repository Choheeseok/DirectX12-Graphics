#include "Common.hlsl"

struct VertexIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VertexOut
{
	float4 position : SV_Position;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	nointerpolation uint matIdx : MATIDX;
};

VertexOut VS(VertexIn input, uint instanceID : SV_InstanceID)
{
	VertexOut output;

	output.positionW = mul(float4(input.position, 1.0f),
		gGameObjectInfos[instanceID].m_mtxWorld).xyz;

	output.position = mul(mul(
		float4(output.positionW, 1.0f),
		gmtxView),
		gmtxProjection);

	output.normalW = mul(float4(input.normal, 0.0f),
		gGameObjectInfos[instanceID].m_mtxWorld).xyz;

	output.uv = input.uv;

	output.matIdx = gGameObjectInfos[instanceID].m_nMaterialIndex;

	return output;
}

float4 PS(VertexOut input) : SV_Target
{
	float4 color = gtxtDiffuseMap.Sample(gSamplerState,input.uv);
	input.normalW = normalize(input.normalW);

	return color * Lighting(input.matIdx, input.positionW, input.normalW);
}