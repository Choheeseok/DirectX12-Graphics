#include "pch.h"
#include "Texture.h"

Texture::Texture(UINT nTextures, int nRootParameterIndex) :
	m_nTextures{ nTextures },
	m_nSrvHeapIndex{ -1 },
	m_nRootParameterIndex{ nRootParameterIndex }
{
	m_vTextures.resize(m_nTextures);
	m_vTextureUploadBuffers.resize(m_nTextures);
}

void Texture::LoadTextureFromFile(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_nResourceType = nResourceType;
	m_vTextures[nIndex] = ::CreateTextureResourceFromFile(
		pd3dDevice, pd3dCommandList, pszFileName,
		&m_vTextureUploadBuffers[nIndex],
		D3D12_RESOURCE_STATE_GENERIC_READ);
}