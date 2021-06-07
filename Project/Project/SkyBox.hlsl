#include "Shaders.hlsl"

TextureCube gtxtCubeMap : register(t3);

struct VS_SKYBOX_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_SKYBOX_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
};

struct PS_SKYBOX_OUTPUT
{
	float4 color : SV_Target;
	float depth : SV_DEPTH;
};

VS_SKYBOX_OUTPUT VS_SkyBox(VS_SKYBOX_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_SKYBOX_OUTPUT output;
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

PS_SKYBOX_OUTPUT PS_SkyBox(VS_SKYBOX_OUTPUT input)
{
	PS_SKYBOX_OUTPUT output;
	output.color = gtxtCubeMap.Sample(gSamplerState, input.posW);
	output.depth = 1.0f;

	return output;
}