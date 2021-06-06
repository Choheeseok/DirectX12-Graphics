#include "pch.h"
#include "CubeMesh.h"

CubeMesh::CubeMesh(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nStride = sizeof(Vertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nVertices = 24;
	m_vVertices.resize(m_nVertices);

	float size = 0.5f;
	m_vVertices[0] = Vertex(XMFLOAT3(-size, -size, -size), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
	m_vVertices[1] = Vertex(XMFLOAT3(-size, +size, -size), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	m_vVertices[2] = Vertex(XMFLOAT3(+size, +size, -size), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	m_vVertices[3] = Vertex(XMFLOAT3(+size, -size, -size), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
	m_vVertices[4] = Vertex(XMFLOAT3(-size, -size, +size), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	m_vVertices[5] = Vertex(XMFLOAT3(+size, -size, +size), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));
	m_vVertices[6] = Vertex(XMFLOAT3(+size, +size, +size), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	m_vVertices[7] = Vertex(XMFLOAT3(-size, +size, +size), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f));
	m_vVertices[8] = Vertex(XMFLOAT3(-size, +size, -size), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	m_vVertices[9] = Vertex(XMFLOAT3(-size, +size, +size), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	m_vVertices[10] = Vertex(XMFLOAT3(+size, +size, +size), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	m_vVertices[11] = Vertex(XMFLOAT3(+size, +size, -size), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	m_vVertices[12] = Vertex(XMFLOAT3(-size, -size, -size), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	m_vVertices[13] = Vertex(XMFLOAT3(+size, -size, -size), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	m_vVertices[14] = Vertex(XMFLOAT3(+size, -size, +size), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	m_vVertices[15] = Vertex(XMFLOAT3(-size, -size, +size), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	m_vVertices[16] = Vertex(XMFLOAT3(-size, -size, +size), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	m_vVertices[17] = Vertex(XMFLOAT3(-size, +size, +size), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	m_vVertices[18] = Vertex(XMFLOAT3(-size, +size, -size), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	m_vVertices[19] = Vertex(XMFLOAT3(-size, -size, -size), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	m_vVertices[20] = Vertex(XMFLOAT3(+size, -size, -size), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	m_vVertices[21] = Vertex(XMFLOAT3(+size, +size, -size), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	m_vVertices[22] = Vertex(XMFLOAT3(+size, +size, +size), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	m_vVertices[23] = Vertex(XMFLOAT3(+size, -size, +size), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

	//m_vVertices[0] = Vertex(XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
	//m_vVertices[1] = Vertex(XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	//m_vVertices[2] = Vertex(XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f));
	//m_vVertices[3] = Vertex(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

	m_pd3dVertexBuffer = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		m_vVertices.data(), m_nStride * m_nVertices,
		m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	vector<UINT> vIndices{
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		8,9,10,
		8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23
	};
	m_vIndices = vIndices;

	m_pd3dIndexBuffer = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		m_vIndices.data(), sizeof(UINT) * m_nIndices,
		m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}