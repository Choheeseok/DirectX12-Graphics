#include "pch.h"
#include "d3dUtil.h"
#include "Mesh.h"

void VertexBuffer::Create(
	ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nStride, UINT nVertices, const void* pData)
{
	m_pd3dResource = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		pData, nStride * nVertices,
		m_pd3dUploader);
	m_d3dVertexBufferView.BufferLocation = m_pd3dResource->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = nStride;
	m_d3dVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * nVertices;nStride* nVertices;
}

void Mesh::Render(
	ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pd3dVertexBufferViews[3] = {
		m_umpVertexBuffers["position"]->GetViewRef(),
		m_umpVertexBuffers["normal"]->GetViewRef(),
		m_umpVertexBuffers["uv0"]->GetViewRef(),
	};
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 3, pd3dVertexBufferViews);

	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(
			m_vIndices.size(), nInstances, m_nStartIndex, m_nBaseVertex, 0);
	}
	else {
		pd3dCommandList->DrawInstanced(
			m_nVertices, nInstances, m_nOffset, 0);
	}
}