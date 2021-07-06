#include "Common.hlsl"

struct VertexIn
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
};

struct PixelOut
{
	float4 color : SV_Target;
	float depth : SV_DEPTH;
};

VertexOut VS(VertexIn input, uint instanceID : SV_InstanceID)
{
	VertexOut output;
	matrix mtxWorld = matrix(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z, 1);

	output.posW = mul(
		float4(input.pos, 1.0f),
		gGameObjectInfos[instanceID].m_mtxWorld).xyz;

	output.pos = mul(mul(mul(
		float4(input.pos, 1.0f),
		mtxWorld),
		gmtxView),
		gmtxProjection);
	output.pos.z = output.pos.w;

	return output;
}

PixelOut PS(VertexOut input)
{
	PixelOut output;
	output.color = gtxtCubeMap.Sample(gSamplerState, input.posW);
	output.depth = 1.0f;

	return output;
}