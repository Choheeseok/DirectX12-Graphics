struct INSTANCE_GAMEOBJECT_INFO
{
	matrix m_mtxWorld;
};

cbuffer cbCameraInfo : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
};

StructuredBuffer<INSTANCE_GAMEOBJECT_INFO> gGameObjectInfos : register(t0);

SamplerState gSamplerState : register(s0);

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
	float2 uv : TEXCOORD;
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

	output.uv = input.uv;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = float4(input.positionW,1);

	return color;
}