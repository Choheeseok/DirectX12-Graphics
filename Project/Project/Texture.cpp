#include "pch.h"
#include "d3dUtil.h"
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

void Texture::CreateTextureResource(
	ID3D12Device* pd3dDevice,
	UINT nWidth,
	UINT nHeight,
	DXGI_FORMAT dxgiFormat,
	D3D12_RESOURCE_FLAGS d3dFlags,
	D3D12_RESOURCE_STATES d3dResourceState,
	UINT nIndex)
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nWidth;
	d3dResourceDesc.Height = nHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = dxgiFormat;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = d3dFlags;

	ThrowIfFailed(pd3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc,
		d3dResourceState,
		nullptr,
		IID_PPV_ARGS(m_vTextures[nIndex].GetAddressOf())));
}