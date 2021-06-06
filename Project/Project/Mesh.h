#pragma once

#include "d3dUtil.h"

class Vertex
{
private:
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT2 m_xmf2TexCoord0;

public:
	Vertex() :
		m_xmf3Position{ },
		m_xmf3Normal{},
		m_xmf2TexCoord0{}{};
	Vertex(
		const XMFLOAT3& xmf3Position,
		const XMFLOAT3& xmf3Normal,
		const XMFLOAT2& xmf2TexCoord0)
		: m_xmf3Position{ xmf3Position },
		m_xmf3Normal{ xmf3Normal },
		m_xmf2TexCoord0{ xmf2TexCoord0 }{};
	virtual ~Vertex() {};
};

class Mesh
{
protected:
	string m_sName{};

	UINT m_nVertices{ 0 };
	vector<Vertex> m_vVertices;
	ComPtr<ID3D12Resource> m_pd3dVertexBuffer;
	ComPtr<ID3D12Resource> m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	UINT m_nSlot{ 0 };
	UINT m_nStride{ 0 };
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