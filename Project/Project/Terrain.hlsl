#include "Shaders.hlsl"

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv0 : TEXCOORD0;
};

struct VS_TERRAIN_OUTPUT
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
	nointerpolation uint matIdx : MATIDX;
};

struct HSC_OUTPUT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
	nointerpolation uint matIdx : MATIDX;
};

struct DS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
	nointerpolation uint matIdx : MATIDX;
};

float4 BernsteinCoefficient4x4(float t)
{
	float tInv = 1.0f - t;
	float4 f4Bernstein =
		float4(tInv * tInv * tInv,
			3.0f * t * tInv * tInv,
			3.0f * t * t * tInv,
			t * t * t);

	return f4Bernstein;
}

float3 CubicBezierSum4x4(
	OutputPatch<HS_OUTPUT, 16> patch,
	float4 uB, float4 vB)
{
	float3 sum = float3(0.0f, 0.0f, 0.0f);
	sum += vB.x * (uB.x * patch[0].position + uB.y * patch[1].position + uB.z * patch[2].position + uB.w * patch[3].position);
	sum += vB.y * (uB.x * patch[4].position + uB.y * patch[5].position + uB.z * patch[6].position + uB.w * patch[7].position);
	sum += vB.z * (uB.x * patch[8].position + uB.y * patch[9].position + uB.z * patch[10].position + uB.w * patch[11].position);
	sum += vB.w * (uB.x * patch[12].position + uB.y * patch[13].position + uB.z * patch[14].position + uB.w * patch[15].position);
	return sum;
}

float3 BezierNormalSum4x4(
	OutputPatch<HS_OUTPUT, 16> patch,
	float4 uB, float4 vB)
{
	float3 sum = float3(0.0f, 0.0f, 0.0f);
	sum += vB.x * (uB.x * patch[0].normal + uB.y * patch[1].normal + uB.z * patch[2].normal + uB.w * patch[3].normal);
	sum += vB.y * (uB.x * patch[4].normal + uB.y * patch[5].normal + uB.z * patch[6].normal + uB.w * patch[7].normal);
	sum += vB.z * (uB.x * patch[8].normal + uB.y * patch[9].normal + uB.z * patch[10].normal + uB.w * patch[11].normal);
	sum += vB.w * (uB.x * patch[12].normal + uB.y * patch[13].normal + uB.z * patch[14].normal + uB.w * patch[15].normal);
	return sum;
}

float2 UV0Sum4x4(
	OutputPatch<HS_OUTPUT, 16> patch,
	float4 uB, float4 vB)
{
	float2 sum = float2(0.0f, 0.0f);
	sum += vB.x * (uB.x * patch[0].uv0 + uB.y * patch[1].uv0 + uB.z * patch[2].uv0 + uB.w * patch[3].uv0);
	sum += vB.y * (uB.x * patch[4].uv0 + uB.y * patch[5].uv0 + uB.z * patch[6].uv0 + uB.w * patch[7].uv0);
	sum += vB.z * (uB.x * patch[8].uv0 + uB.y * patch[9].uv0 + uB.z * patch[10].uv0 + uB.w * patch[11].uv0);
	sum += vB.w * (uB.x * patch[12].uv0 + uB.y * patch[13].uv0 + uB.z * patch[14].uv0 + uB.w * patch[15].uv0);
	return sum;
}

float2 UV1Sum4x4(
	OutputPatch<HS_OUTPUT, 16> patch,
	float4 uB, float4 vB)
{
	float2 sum = float2(0.0f, 0.0f);
	sum += vB.x * (uB.x * patch[0].uv1 + uB.y * patch[1].uv1 + uB.z * patch[2].uv1 + uB.w * patch[3].uv1);
	sum += vB.y * (uB.x * patch[4].uv1 + uB.y * patch[5].uv1 + uB.z * patch[6].uv1 + uB.w * patch[7].uv1);
	sum += vB.z * (uB.x * patch[8].uv1 + uB.y * patch[9].uv1 + uB.z * patch[10].uv1 + uB.w * patch[11].uv1);
	sum += vB.w * (uB.x * patch[12].uv1 + uB.y * patch[13].uv1 + uB.z * patch[14].uv1 + uB.w * patch[15].uv1);
	return sum;
}

VS_TERRAIN_OUTPUT VS_Terrain(VS_TERRAIN_INPUT input, uint i : SV_InstanceID)
{
	VS_TERRAIN_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), gGameObjectInfos[i].m_mtxWorld).xyz;
	output.uv0 = input.uv0;
	output.uv1 = input.uv0;
	output.normal = mul(float4(input.normal, 0.0f), gGameObjectInfos[i].m_mtxWorld).xyz;
	output.matIdx = gGameObjectInfos[i].m_nMaterialIndex;

	return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(16)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS_Terrain(
	InputPatch<VS_TERRAIN_OUTPUT, 16> input,
	uint i : SV_OutputControlPointID)
{
	HS_OUTPUT output = (HS_OUTPUT)0;
	output.position = input[i].position;
	output.uv0 = input[i].uv0;
	output.uv1 = input[i].uv1;
	output.normal = input[i].normal;
	output.matIdx = input[i].matIdx;

	return output;
}

HSC_OUTPUT ConstantHS(
	InputPatch<VS_TERRAIN_OUTPUT, 16> input,
	uint nPatchID : SV_PrimitiveID)
{
	HSC_OUTPUT output;
	output.fTessEdges[0] = 8.0f;
	output.fTessEdges[1] = 8.0f;
	output.fTessEdges[2] = 8.0f;
	output.fTessEdges[3] = 8.0f;
	output.fTessInsides[0] = 8.0f;
	output.fTessInsides[1] = 8.0f;
	return output;
}

[domain("quad")]
DS_OUTPUT DS_Terrain(
	HSC_OUTPUT input,
	float2 uv : SV_DomainLocation,
	OutputPatch<HS_OUTPUT, 16> patch)
{
	DS_OUTPUT output;
	float4 uB = BernsteinCoefficient4x4(uv.x);
	float4 vB = BernsteinCoefficient4x4(uv.y);
	float3 position = CubicBezierSum4x4(patch, uB, vB);
	float3 normal = BezierNormalSum4x4(patch, uB, vB);
	normal = normalize(normal);
	float2 uv0 = UV0Sum4x4(patch, uB, vB);
	float2 uv1 = UV1Sum4x4(patch, uB, vB);

	output.position = mul(mul(
		float4(position, 1.0f),
		gmtxView),
		gmtxProjection);
	output.normal = normal;
	output.uv0 = uv0;
	output.uv1 = uv1;
	output.positionW = position;
	output.matIdx = patch[0].matIdx;

	return output;
}

float4 PS_Terrain(DS_OUTPUT input) :SV_Target
{
	float4 cBaseTexColor = gtxtDiffuseMap.Sample(gSamplerState,input.uv0);

	input.normal = normalize(input.normal);
	float4 cLightColor = Lighting(input.matIdx, input.positionW, input.normal);

	float4 cColor = cBaseTexColor * cLightColor;
	cColor.w = 1.0f;

	return cColor;
}