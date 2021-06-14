#define MAX_LIGHTS			16

#define DIRECTIONAL_LIGHT	1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3

struct INSTANCE_GAMEOBJECT_INFO
{
	matrix m_mtxWorld;
	uint m_nMaterialIndex;
};

struct INSTANCE_MATERIAL_INFO
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
	float4 m_cEmissive;
};

struct LIGHT
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular;
	float3					m_vPosition;
	float 					m_fFalloff;
	float3					m_vDirection;
	float 					m_fTheta; //cos(m_fTheta)
	float3					m_vAttenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int 					m_nType;
	float					m_fRange;
	float					padding;
};

cbuffer cbCameraInfo : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbLights : register(b1)
{
	LIGHT					gLights[MAX_LIGHTS];
	int						gnLights;
};

StructuredBuffer<INSTANCE_GAMEOBJECT_INFO> gGameObjectInfos : register(t0);

StructuredBuffer<INSTANCE_MATERIAL_INFO> gMaterialInfos : register(t1);

Texture2D gtxtDiffuseMap : register(t2);
SamplerState gSamplerState : register(s0);

TextureCube gtxtCubeMap : register(t3);

#include "Light.hlsl"

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	nointerpolation uint matIdx : MATIDX;
};

VS_OUTPUT VS(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;

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

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = gtxtDiffuseMap.Sample(gSamplerState,input.uv);
	input.normalW = normalize(input.normalW);

	return color * Lighting(input.matIdx, input.positionW, input.normalW);
}