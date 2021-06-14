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