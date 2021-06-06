#include "pch.h"
#include "d3dUtil.h"
#include "Mesh.h"

void Mesh::Render(
	ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	pd3dCommandList->DrawInstanced(3, 1, 0, 0);
	//pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	//if (m_pd3dIndexBuffer) {
	//	pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
	//	pd3dCommandList->DrawIndexedInstanced(
	//		m_vIndices.size(), nInstances, m_nStartIndex, m_nBaseVertex, 0);
	//}
	//else {
	//	pd3dCommandList->DrawInstanced(
	//		m_vVertices.size(), nInstances, m_nOffset, 0);
	//}
}