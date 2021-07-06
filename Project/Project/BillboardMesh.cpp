#include "pch.h"
#include "BillboardMesh.h"

BillboardMesh::BillboardMesh(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	float fSizeX = 0.5f;
	float fSizeY = 0.5f;

	m_nVertices = 1;
	m_vxmf3Positions.emplace_back(0.0f, 0.0f, 0.0f);
	m_vxmf3Normals.emplace_back(0.0f, 0.0f, -1.0f);
	m_vxmf2TexCoords0.emplace_back(fSizeX, fSizeY);

	unique_ptr<VertexBuffer> pPositionBuffer = make_unique<VertexBuffer>();
	pPositionBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT3), m_nVertices, m_vxmf3Positions.data());
	m_umpVertexBuffers["position"] = move(pPositionBuffer);

	unique_ptr<VertexBuffer> pNormalBuffer = make_unique<VertexBuffer>();
	pNormalBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT3), m_nVertices, m_vxmf3Normals.data());
	m_umpVertexBuffers["normal"] = move(pNormalBuffer);

	unique_ptr<VertexBuffer> pTexCoordBuffer = make_unique<VertexBuffer>();
	pTexCoordBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT2), m_nVertices, m_vxmf2TexCoords0.data());
	m_umpVertexBuffers["uv0"] = move(pTexCoordBuffer);
}