#include "pch.h"
#include "Material.h"

void Material::SetShaderVariables(
	ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12DescriptorHeap* pd3dDescriptorHeap,
	UINT nCbvSrvUavDescriptorIncrementSize)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE d3dTextureSrvHandle{
			pd3dDescriptorHeap->GetGPUDescriptorHandleForHeapStart() };
	d3dTextureSrvHandle.Offset(m_pTexture->m_nSrvHeapIndex, nCbvSrvUavDescriptorIncrementSize);

	pd3dCommandList->SetGraphicsRootDescriptorTable(
		m_pTexture->m_nRootParameterIndex, d3dTextureSrvHandle);
}