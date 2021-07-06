#include "Common.hlsl"

struct VertexIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv0 : TEXCOORD0;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
	nointerpolation uint matIdx : MATIDX;
};

VertexOut VS(VertexIn input, uint i : SV_InstanceID)
{
	VertexOut output;
	output.positionW = mul(float4(input.position, 1.0f), gGameObjectInfos[i].m_mtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv0 = input.uv0;
	output.uv1 = input.uv0;
	output.normal = mul(float4(input.normal, 0.0f), gGameObjectInfos[i].m_mtxWorld).xyz;
	output.matIdx = gGameObjectInfos[i].m_nMaterialIndex;

	return output;
}

float4 PS(VertexOut input) :SV_Target
{
	float4 cBaseTexColor = gtxtDiffuseMap.Sample(gSamplerState,input.uv0);

	input.normal = normalize(input.normal);
	float4 cLightColor = Lighting(input.matIdx, input.positionW, input.normal);

	float4 cColor = cBaseTexColor * cLightColor;
	cColor.w = 1.0f;

	return cColor;
}