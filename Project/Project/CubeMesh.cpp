#include "pch.h"
#include "CubeMesh.h"

CubeMesh::CubeMesh(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nVertices = 24;
	m_vxmf3Positions.resize(m_nVertices);
	m_vxmf3Normals.resize(m_nVertices);
	m_vxmf2TexCoords0.resize(m_nVertices);

	float size = 0.5f;
	m_vxmf3Positions[0] = XMFLOAT3(-size, -size, -size), m_vxmf3Normals[0] = XMFLOAT3(0.0f, 0.0f, -1.0f), m_vxmf2TexCoords0[0] =  XMFLOAT2(0.0f, 1.0f);
	m_vxmf3Positions[1] = XMFLOAT3(-size, +size, -size), m_vxmf3Normals[1] = XMFLOAT3(0.0f, 0.0f, -1.0f), m_vxmf2TexCoords0[1] = XMFLOAT2(0.0f, 0.0f);
	m_vxmf3Positions[2] = XMFLOAT3(+size, +size, -size), m_vxmf3Normals[2] = XMFLOAT3(0.0f, 0.0f, -1.0f), m_vxmf2TexCoords0[2] = XMFLOAT2(1.0f, 0.0f);
	m_vxmf3Positions[3] = XMFLOAT3(+size, -size, -size), m_vxmf3Normals[3] = XMFLOAT3(0.0f, 0.0f, -1.0f), m_vxmf2TexCoords0[3] = XMFLOAT2(1.0f, 1.0f);
	m_vxmf3Positions[4] = XMFLOAT3(-size, -size, +size), m_vxmf3Normals[4] = XMFLOAT3(0.0f, 0.0f, 1.0f), m_vxmf2TexCoords0[4] = XMFLOAT2(1.0f, 1.0f);
	m_vxmf3Positions[5] = XMFLOAT3(+size, -size, +size), m_vxmf3Normals[5] = XMFLOAT3(0.0f, 0.0f, 1.0f), m_vxmf2TexCoords0[5] = XMFLOAT2(0.0f, 1.0f);
	m_vxmf3Positions[6] = XMFLOAT3(+size, +size, +size), m_vxmf3Normals[6] = XMFLOAT3(0.0f, 0.0f, 1.0f), m_vxmf2TexCoords0[6] = XMFLOAT2(0.0f, 0.0f);
	m_vxmf3Positions[7] = XMFLOAT3(-size, +size, +size), m_vxmf3Normals[7] = XMFLOAT3(0.0f, 0.0f, 1.0f), m_vxmf2TexCoords0[7] = XMFLOAT2(1.0f, 0.0f);
	m_vxmf3Positions[8] = XMFLOAT3(-size, +size, -size), m_vxmf3Normals[8] = XMFLOAT3(0.0f, 1.0f, 0.0f), m_vxmf2TexCoords0[8] = XMFLOAT2(0.0f, 1.0f);
	m_vxmf3Positions[9] = XMFLOAT3(-size, +size, +size), m_vxmf3Normals[9] = XMFLOAT3(0.0f, 1.0f, 0.0f), m_vxmf2TexCoords0[9] = XMFLOAT2(0.0f, 0.0f);
	m_vxmf3Positions[10] = XMFLOAT3(+size, +size, +size), m_vxmf3Normals[10] = XMFLOAT3(0.0f, 1.0f, 0.0f), m_vxmf2TexCoords0[10] = XMFLOAT2(1.0f, 0.0f);
	m_vxmf3Positions[11] = XMFLOAT3(+size, +size, -size), m_vxmf3Normals[11] = XMFLOAT3(0.0f, 1.0f, 0.0f), m_vxmf2TexCoords0[11] = XMFLOAT2(1.0f, 1.0f);
	m_vxmf3Positions[12] = XMFLOAT3(-size, -size, -size), m_vxmf3Normals[12] = XMFLOAT3(0.0f, -1.0f, 0.0f), m_vxmf2TexCoords0[12] = XMFLOAT2(1.0f, 1.0f);
	m_vxmf3Positions[13] = XMFLOAT3(+size, -size, -size), m_vxmf3Normals[13] = XMFLOAT3(0.0f, -1.0f, 0.0f), m_vxmf2TexCoords0[13] = XMFLOAT2(0.0f, 1.0f);
	m_vxmf3Positions[14] = XMFLOAT3(+size, -size, +size), m_vxmf3Normals[14] = XMFLOAT3(0.0f, -1.0f, 0.0f), m_vxmf2TexCoords0[14] = XMFLOAT2(0.0f, 0.0f);
	m_vxmf3Positions[15] = XMFLOAT3(-size, -size, +size), m_vxmf3Normals[15] = XMFLOAT3(0.0f, -1.0f, 0.0f), m_vxmf2TexCoords0[15] = XMFLOAT2(1.0f, 0.0f);
	m_vxmf3Positions[16] = XMFLOAT3(-size, -size, +size), m_vxmf3Normals[16] = XMFLOAT3(-1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[16] = XMFLOAT2(0.0f, 1.0f);
	m_vxmf3Positions[17] = XMFLOAT3(-size, +size, +size), m_vxmf3Normals[17] = XMFLOAT3(-1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[17] = XMFLOAT2(0.0f, 0.0f);
	m_vxmf3Positions[18] = XMFLOAT3(-size, +size, -size), m_vxmf3Normals[18] = XMFLOAT3(-1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[18] = XMFLOAT2(1.0f, 0.0f);
	m_vxmf3Positions[19] = XMFLOAT3(-size, -size, -size), m_vxmf3Normals[19] = XMFLOAT3(-1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[19] = XMFLOAT2(1.0f, 1.0f);
	m_vxmf3Positions[20] = XMFLOAT3(+size, -size, -size), m_vxmf3Normals[20] = XMFLOAT3(1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[20] = XMFLOAT2(0.0f, 1.0f);
	m_vxmf3Positions[21] = XMFLOAT3(+size, +size, -size), m_vxmf3Normals[21] = XMFLOAT3(1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[21] = XMFLOAT2(0.0f, 0.0f);
	m_vxmf3Positions[22] = XMFLOAT3(+size, +size, +size), m_vxmf3Normals[22] = XMFLOAT3(1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[22] = XMFLOAT2(1.0f, 0.0f);
	m_vxmf3Positions[23] = XMFLOAT3(+size, -size, +size), m_vxmf3Normals[23] = XMFLOAT3(1.0f, 0.0f, 0.0f), m_vxmf2TexCoords0[23] = XMFLOAT2(1.0f, 1.0f);

	unique_ptr<VertexBuffer> pPositionBuffer = make_unique<VertexBuffer>();
	pPositionBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT3), m_nVertices, m_vxmf3Positions.data());
	m_umpVertexBuffers["position"] = move(pPositionBuffer);

	unique_ptr<VertexBuffer> pNormalBuffer = make_unique<VertexBuffer>();
	pNormalBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT3), m_nVertices, m_vxmf3Normals.data());
	m_umpVertexBuffers["normal"] = move(pNormalBuffer);

	unique_ptr<VertexBuffer> pTexCoordBuffer = make_unique<VertexBuffer>();
	pTexCoordBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT2), m_nVertices, m_vxmf2TexCoords0.data());
	m_umpVertexBuffers["uv0"] = move(pTexCoordBuffer);

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