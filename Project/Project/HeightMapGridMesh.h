#pragma once
#include "Mesh.h"

class HeightMapImage
{
private:
	BYTE* m_pHeightMapPixels;

	int m_nWidth;
	int m_nLength;
	XMFLOAT3 m_xmf3Scale;

public:
	HeightMapImage(
		LPCTSTR pFileName,
		int nWidth, int nLength,
		XMFLOAT3 xmf3Scale);
	~HeightMapImage();

	BYTE* GetHeightMapPixels() { return m_pHeightMapPixels; };
	XMFLOAT3 GetScale() { return m_xmf3Scale; };
	int GetHeightMapWidth() { return m_nWidth; };
	int GetHeightMapLength() { return m_nLength; };

	float GetHeight(const float& fx, const float& fz) const;
	XMFLOAT3 GetHeightMapNormal(
		const int& x, const int& z) const;
};

class HeightMapGridMesh : public Mesh
{
protected:
	int m_nWidth;
	int m_nLength;
	XMFLOAT3 m_xmf3Scale;

public:
	HeightMapGridMesh(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		int xStart, int zStart, int nWidth, int nLength,
		XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color,
		void* pContext);
	virtual ~HeightMapGridMesh();

	XMFLOAT3 GetScale() { return m_xmf3Scale; };
	int GetWidth() { return m_nWidth; };
	int GetLength() { return m_nLength; };

	virtual float GetHeight(int x, int z, void* pContext);
	virtual XMFLOAT3 GetNormal(int x, int z, void* pContext);
	virtual XMFLOAT4 GetColor(int x, int z, void* pContext);
};
