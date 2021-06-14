float4 DirectionalLight(uint nMatIdx, int nIndex, float3 vNormal, float3 vToCamera)
{
	float3 vToLight = -gLights[nIndex].m_vDirection;
	float fDiffuseFactor = dot(vToLight, vNormal);
	float fSpecularFactor = 0.0f;
	if (fDiffuseFactor > 0.0f)
	{
		if (gMaterialInfos[nMatIdx].m_cSpecular.a != 0.0f)
		{
			float3 vHalf = normalize(vToCamera + vToLight);
			fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterialInfos[nMatIdx].m_cSpecular.a);
		}
	}

	return((gLights[nIndex].m_cAmbient * gMaterialInfos[nMatIdx].m_cAmbient) +
		(gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterialInfos[nMatIdx].m_cDiffuse) +
		(gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterialInfos[nMatIdx].m_cSpecular));
}

float4 PointLight(int nMatIdx, int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	if (fDistance <= gLights[nIndex].m_fRange) {
		float fSpecularFactor = 0.0f;
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);

		if (fDiffuseFactor > 0.0f) {
			if (gMaterialInfos[nMatIdx].m_cSpecular.a != 0.0f) {
				float3 vHalf = normalize(vToCamera + vToLight);
				fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterialInfos[nMatIdx].m_cSpecular.a);
			}
		}

		float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

		cColor = ((gLights[nIndex].m_cAmbient * gMaterialInfos[nMatIdx].m_cAmbient) +
			(gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterialInfos[nMatIdx].m_cDiffuse) +
			(gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterialInfos[nMatIdx].m_cSpecular)) *
			fAttenuationFactor;
	}
	return cColor;
}

float4 SpotLight(int nMatIdx, int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
	float fDistance = length(vToLight);
	if (fDistance <= gLights[nIndex].m_fRange) {
		float fSpecularFactor = 0.0f;
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		if (fDiffuseFactor > 0.0f)
		{
			if (gMaterialInfos[nMatIdx].m_cSpecular.a != 0.0f)
			{
				float3 vHalf = normalize(vToCamera + vToLight);
				fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterialInfos[nMatIdx].m_cSpecular.a);
			}
		}
		float fAlpha = max(dot(-vToLight, gLights[nIndex].m_vDirection), 0.0f);
		float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) / (gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);
		float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

		cColor = ((gLights[nIndex].m_cAmbient * gMaterialInfos[nMatIdx].m_cAmbient) +
			(gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterialInfos[nMatIdx].m_cDiffuse) +
			(gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterialInfos[nMatIdx].m_cSpecular)) *
			fAttenuationFactor * fSpotFactor;
	}
	return cColor;
}

float4 Lighting(uint nMatIdx, float3 vPosition, float3 vNormal)
{
	float3 vCameraPosition = gvCameraPosition;
	float3 vToCamera = normalize(vCameraPosition - vPosition);

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	[unroll(MAX_LIGHTS)]
	for (int i = 0; i < gnLights; i++)
	{
		if (gLights[i].m_bEnable) {
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
				cColor += DirectionalLight(nMatIdx, i, vNormal, vToCamera);
			else if (gLights[i].m_nType == POINT_LIGHT)
				cColor += PointLight(nMatIdx, i, vPosition, vNormal, vToCamera);
			else if (gLights[i].m_nType == SPOT_LIGHT)
				cColor += SpotLight(nMatIdx, i, vPosition, vNormal, vToCamera);
		}
	}
	cColor += gMaterialInfos[nMatIdx].m_cAmbient;
	cColor.a = gMaterialInfos[nMatIdx].m_cDiffuse.a;

	return(cColor);
}