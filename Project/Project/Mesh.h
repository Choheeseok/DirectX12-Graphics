#pragma once

#include "d3dUtil.h"

class VertexBuffer
{
private:
	ComPtr<ID3D12Resource> m_pd3dResource;
	ComPtr<ID3D12Resource> m_pd3dUploader;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
public:
	ID3D12Resource* Get() { return m_pd3dResource.Get(); }
	ID3D12Resource* GetUploader() { return m_pd3dUploader.Get(); }
	D3D12_VERTEX_BUFFER_VIEW& GetViewRef() { return m_d3dVertexBufferView; };

	void Create(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nStride, UINT nVertices, const void* pData);
};

class Mesh
{
protected:
	string m_sName{};

	UINT m_nVertices{ 0 };
	vector<XMFLOAT3> m_vxmf3Positions;
	vector<XMFLOAT3> m_vxmf3Normals;
	vector<XMFLOAT2> m_vxmf2TexCoords0;
	vector<XMFLOAT2> m_vxmf2TexCoords1;

	unordered_map<string, unique_ptr<VertexBuffer>> m_umpVertexBuffers;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	UINT m_nSlot{ 0 };
	UINT m_nOffset{ 0 };

	UINT m_nIndices{ 0 };
	vector<UINT> m_vIndices;
	ComPtr<ID3D12Resource> m_pd3dIndexBuffer;
	ComPtr<ID3D12Resource> m_pd3dIndexUploadBuffer;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	UINT m_nStartIndex{ 0 };
	int m_nBaseVertex{ 0 };

public:
	void Render(
		ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances);
};