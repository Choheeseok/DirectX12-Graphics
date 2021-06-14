#include "pch.h"
#include "TerrainObject.h"

TerrainObject::TerrainObject(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature,
	LPCTSTR pFileName,
	int nWidth, int nLength, int nBlockWidth, int nBlockLength,
	XMFLOAT3 xmf3Scale)
	: GameObject{ 0, 0 },
	m_nWidth{ nWidth }, m_nLength{ nLength },
	m_xmf3Scale{ xmf3Scale }
{
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_pHeightMapImage = new HeightMapImage(
		pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_vMeshes.resize(cxBlocks* czBlocks);
	HeightMapGridMesh* pHeightMapGridMesh = nullptr;
	for (int z = 0, zStart = 0; z < czBlocks; z++) {
		for (int x = 0, xStart = 0; x < cxBlocks; x++) {
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);

			pHeightMapGridMesh = new HeightMapGridMesh(
				pd3dDevice, pd3dCommandList,
				xStart, zStart, nBlockWidth, nBlockLength,
				xmf3Scale, m_pHeightMapImage);

			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}
}

TerrainObject::~TerrainObject()
{
	if (m_pHeightMapImage)
		delete m_pHeightMapImage;
}

float TerrainObject::GetHeight(float x, float z) const
{
	return m_pHeightMapImage->GetHeight(
		x / m_xmf3Scale.x, z / m_xmf3Scale.z);
}

XMFLOAT3 TerrainObject::GetNormal(float x, float z) const
{
	return m_pHeightMapImage->GetHeightMapNormal(
		int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z));
}

void TerrainObject::SetMaterial(int nIndex, Material* pMaterial)
{
	m_vMaterials.emplace_back(pMaterial);
}

void TerrainObject::Render(
	ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12DescriptorHeap* pd3dDescriptorHeap,
	UINT nCbvSrvUavDescriptorIncrementSize,
	UINT nInstances)
{
	m_vMaterials[0]->SetShaderVariables(
		pd3dCommandList, pd3dDescriptorHeap, nCbvSrvUavDescriptorIncrementSize);
	for (int i = 0; i < m_vMeshes.size(); i++) {
		m_vMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}