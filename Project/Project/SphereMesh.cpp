#include "pch.h"
#include "SphereMesh.h"

SphereMesh::SphereMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nStride = sizeof(Vertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	int nSliceCnt = 20;
	int nStackCnt = 20;
	float fRadius = 0.5f;

	Vertex topVertex{
		XMFLOAT3(0.0f, fRadius, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f) };
	m_vVertices.emplace_back(topVertex);

	float fPhiStep = XM_PI / nStackCnt;
	float fThetaStep = 2.0f * XM_PI / nSliceCnt;

	for (int i = 1; i <= nStackCnt - 1; i++) {
		float fPhi = i * fPhiStep;

		for (int j = 0; j <= nSliceCnt; j++) {
			float fTheta = j * fThetaStep;

			XMFLOAT3 xmf3Position;
			xmf3Position.x = fRadius * sinf(fPhi) * cosf(fTheta);
			xmf3Position.y = fRadius * cosf(fPhi);
			xmf3Position.z = fRadius * sinf(fPhi) * sinf(fTheta);

			XMFLOAT3 xmf3Normal;
			xmf3Normal = Vector3::Normalize(xmf3Position);

			XMFLOAT2 xmf2UV;
			xmf2UV.x = fTheta / XM_PI;
			xmf2UV.y = fPhi / XM_PI;

			m_vVertices.emplace_back(Vertex{ xmf3Position, xmf3Normal, xmf2UV });
		}
	}
	Vertex bottomVertex{
		XMFLOAT3(0.0f, -fRadius, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f) };
	m_vVertices.emplace_back(bottomVertex);

	m_nVertices = m_vVertices.size();

	m_pd3dVertexBuffer = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		m_vVertices.data(), sizeof(Vertex) * m_nVertices,
		m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	for (UINT i = 1; i <= nSliceCnt; i++) {
		m_vIndices.emplace_back(0);
		m_vIndices.emplace_back(i + 1);
		m_vIndices.emplace_back(i);
	}

	UINT nBaseIndex = 1;
	UINT nRingVertexCnt = nSliceCnt + 1;
	for (UINT i = 0; i < nStackCnt - 2; i++) {
		for (UINT j = 0; j < nSliceCnt; j++) {
			m_vIndices.emplace_back(nBaseIndex + i * nRingVertexCnt + j);
			m_vIndices.emplace_back(nBaseIndex + i * nRingVertexCnt + j + 1);
			m_vIndices.emplace_back(nBaseIndex + (i + 1) * nRingVertexCnt + j);
			m_vIndices.emplace_back(nBaseIndex + (i + 1) * nRingVertexCnt + j);
			m_vIndices.emplace_back(nBaseIndex + i * nRingVertexCnt + j + 1);
			m_vIndices.emplace_back(nBaseIndex + (i + 1) * nRingVertexCnt + j + 1);
		}
	}

	UINT nSouthPoleIndex = m_vVertices.size() - 1;

	nBaseIndex = nSouthPoleIndex - nRingVertexCnt;

	for (UINT i = 0; i < nSliceCnt; i++) {
		m_vIndices.emplace_back(nSouthPoleIndex);
		m_vIndices.emplace_back(nBaseIndex + i);
		m_vIndices.emplace_back(nBaseIndex + i + 1);
	}

	m_nIndices = m_vIndices.size();

	m_pd3dIndexBuffer = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		m_vIndices.data(), sizeof(UINT) * m_vIndices.size(),
		m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_vIndices.size();
}