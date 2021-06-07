#pragma once

class Light
{
public:
	static const UINT MAX_LIGHT = 16;
	enum TYPE {
		DirectionalLight = 1
	};

	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;

public:
	Light() { ZeroMemory(this, sizeof(Light)); };
	~Light() {};
};

struct CB_LIGHT_INFO
{
	Light m_pLights[Light::MAX_LIGHT];
	UINT m_nLights;
};