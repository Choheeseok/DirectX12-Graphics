#pragma once

class Texture
{
public:
	enum ResourceType {
		RESOURCE_TEXTURE2D = 1,
		RESOURCE_TEXTURE2D_ARRAY,
		RESOURCE_TEXTURE2DARRAY,
		RESOURCE_TEXTURE_CUBE,
		RESOURCE_BUFFER
	};
	string m_sName;
	UINT m_nResourceType;
	UINT m_nTextures;
	vector<ComPtr<ID3D12Resource>> m_vTextures;
	vector<ComPtr<ID3D12Resource>> m_vTextureUploadBuffers;

	int m_nSrvHeapIndex;
	int m_nRootParameterIndex;
public:
	Texture(UINT nTextures, int nRootParameterIndex);
	virtual ~Texture() {};

	void LoadTextureFromFile(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		wchar_t* pszFileName,
		UINT nResrouceType,
		UINT nIndex);

	void CreateTextureResource(
		ID3D12Device* pd3dDevice,
		UINT nWidth,
		UINT nHeight,
		DXGI_FORMAT dxgiFormat,
		D3D12_RESOURCE_FLAGS d3dFlags,
		D3D12_RESOURCE_STATES d3dResourceState,
		UINT nIndex);
};
