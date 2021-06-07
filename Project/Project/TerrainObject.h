#pragma once

#include "GameObject.h"
#include "HeightMapGridMesh.h"

class TerrainObject : public GameObject
{
private:
	HeightMapImage* m_pHeightMapImage;

	int m_nWidth;
	int m_nLength;

	XMFLOAT3 m_xmf3Scale;
public:
	TerrainObject(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature,
		LPCTSTR pFileName,
		int nWidth, int nLength, int nBlockWidth, int nBlockLength,
		XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~TerrainObject();

	float GetHeight(float x, float z) const;
	XMFLOAT3 GetNormal(float x, float z) const;

	int GetHeightMapWidth() { return m_pHeightMapImage->GetHeightMapWidth(); }
	int GetHeightMapLength() { return m_pHeightMapImage->GetHeightMapLength(); }
	XMFLOAT3 GetScale() { return m_xmf3Scale; }
	float GetWidth() { return m_nWidth * m_xmf3Scale.x; }
	float GetLength() { return m_nLength * m_xmf3Scale.z; }

	void SetMaterial(int nIndex, Material* pMaterial);

	void Render(ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12DescriptorHeap* pd3dDescriptorHeap,
		UINT nCbvSrvUavDescriptorIncrementSize,
		UINT nInstances) override;
};
