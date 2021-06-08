#include "Shaders.hlsl"

struct VS_BILLBOARD_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 size : SCALE;
};

struct VS_BILLBOARD_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 size : SCALE;
	uint matIdx : MATIDX;
};

struct GS_BILLBOARD_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	uint primID : SV_PrimitiveID;
	uint matIdx : MATIDX;
};

VS_BILLBOARD_OUTPUT VS_Billboard(VS_BILLBOARD_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_BILLBOARD_OUTPUT output;

	output.pos = mul(float4(input.pos, 1.0f),
		gGameObjectInfos[instanceID].m_mtxWorld);

	output.size = input.size;

	output.matIdx = gGameObjectInfos[instanceID].m_nMaterialIndex;

	return output;
}

[maxvertexcount(4)]
void GS_Billboard(
	point VS_BILLBOARD_OUTPUT input[1],
	uint primID :SV_PrimitiveID,
	inout TriangleStream<GS_BILLBOARD_OUTPUT> outStream)
{
	float3 vUp = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition - input[0].pos.xyz;
	vLook.y = 0.0f;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);

	float x = input[0].size.x;
	float y = input[0].size.y;

	GS_BILLBOARD_OUTPUT output;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].pos.xyz + x * vRight - y * vUp, 1.0f);
	pVertices[1] = float4(input[0].pos.xyz + x * vRight + y * vUp, 1.0f);
	pVertices[2] = float4(input[0].pos.xyz - x * vRight - y * vUp, 1.0f);
	pVertices[3] = float4(input[0].pos.xyz - x * vRight + y * vUp, 1.0f);
	float2 pUVs[4] = {
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f) };

	for (int i = 0; i < 4; i++) {
		output.posW = pVertices[i].xyz;
		output.pos = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.uv = pUVs[i];
		output.primID = primID;
		output.matIdx = input[0].matIdx;
		outStream.Append(output);
	}
}

float4 PS_Billboard(GS_BILLBOARD_OUTPUT input) :SV_Target
{
	float4 color = gtxtDiffuseMap.Sample(gSamplerState,input.uv);
	//color = color * Lighting(input.matIdx, input.posW, input.normalW);
	clip(color.a - 0.1f);
	return color;
}