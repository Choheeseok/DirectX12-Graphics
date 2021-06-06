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

//VS_OUTPUT VS(VS_INPUT input, uint instanceID : SV_InstanceID)
//{
//	VS_OUTPUT output;
//
//	output.positionW = mul(float4(input.position, 1.0f),
//		gGameObjectInfos[instanceID].m_mtxWorld).xyz;
//
//	output.position = mul(mul(
//		float4(output.positionW, 1.0f),
//		gmtxView),
//		gmtxProjection);
//
//	output.uv = input.uv;
//
//	return output;
//}
//
//float4 PS(VS_OUTPUT input) : SV_Target
//{
//	float4 color = float4(1,0,0,1);
//
//	return color;
//}

float4 VS(uint nVertexID : SV_VertexID) : SV_Position
{
	float4 output;

if (nVertexID == 0) output = float4(0.0, 0.5, 0.5, 1.0);
else if (nVertexID == 1) output = float4(0.5, -0.5, 0.5, 1.0);
else if (nVertexID == 2) output = float4(-0.5, -0.5, 0.5, 1.0);
return(output);
}

float4 PS(float4 input : SV_Position) : SV_Target
{
	float4 color = float4(1,0,0,1);

	return color;
}