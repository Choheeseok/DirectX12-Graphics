#pragma once

#include "Texture.h"

class Material
{
public:
	string m_sName;

	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT4 m_xmf4Emissive;

	UINT m_nIndex;
	Texture* m_pTexture;

public:
	Material()
		: m_xmf4Ambient{ XMFLOAT4(1.0f, 1.0f,1.0f,1.0f) },
		m_xmf4Diffuse{ XMFLOAT4(1.0f, 1.0f,1.0f,1.0f) },
		m_xmf4Specular{ XMFLOAT4(1.0f, 1.0f,1.0f,1.0f) },
		m_xmf4Emissive{ XMFLOAT4(0.0f,0.0f,0.0f,0.0f) }
	{};
	~Material() {};
	void SetShaderVariables(
		ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12DescriptorHeap* pd3dDescriptorHeap,
		UINT nCbvSrvUavDescriptorIncrementSize);
};

struct INSTANCE_MATERIAL_INFO
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT4 m_xmf4Emissive;
};