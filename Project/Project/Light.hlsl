#define MAX_LIGHTS			16
#define MAX_MATERIALS		512

#define DIRECTIONAL_LIGHT	1

struct INSTANCE_MATERIAL_INFO
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
	float4 m_cEmissive;
};

StructuredBuffer<INSTANCE_MATERIAL_INFO> gMaterialInfos : register(t1);

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

cbuffer cbLights : register(b1)
{
	LIGHT					gLights[MAX_LIGHTS];
	int						gnLights;
};

float4 DirectionalLight(uint nMatIdx, int nIndex, float3 vNormal, float3 vToCamera)
{
	float3 vToLight = -gLights[nIndex].m_vDirection;
	float fDiffuseFactor = dot(vToLight, vNormal);
	float fSpecularFactor = 0.0f;
	if (fDiffuseFactor > 0.0f)
	{
		if (gMaterialInfos[nMatIdx].m_cSpecular.a != 0.0f)
		{
			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
			fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterialInfos[nMatIdx].m_cSpecular.a);
		}
	}

	return((gLights[nIndex].m_cAmbient * gMaterialInfos[nMatIdx].m_cAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterialInfos[nMatIdx].m_cDiffuse) + (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterialInfos[nMatIdx].m_cSpecular));
}

float4 Lighting(uint nMatIdx, float3 vPosition, float3 vNormal)
{
	float3 vCameraPosition = gvCameraPosition;
	float3 vToCamera = normalize(vCameraPosition - vPosition);

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	[unroll(MAX_LIGHTS)] for (int i = 0; i < gnLights; i++)
	{
		if (gLights[i].m_bEnable)
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
				cColor += DirectionalLight(nMatIdx, i, vNormal, vToCamera);
	}
	cColor += gMaterialInfos[nMatIdx].m_cAmbient;
	cColor.a = gMaterialInfos[nMatIdx].m_cDiffuse.a;

	return(cColor);
}