#include "pch.h"
#include "BillboardMesh.h"

BillboardMesh::BillboardMesh(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nStride = sizeof(Vertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	float fSizeX = 0.5f;
	float fSizeY = 0.5f;

	m_nVertices = 1;
	m_vVertices.emplace_back(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT2(fSizeX, fSizeY));

	m_pd3dVertexBuffer = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		m_vVertices.data(), m_nStride * m_nVertices,
		m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}