#include "pch.h"
#include "HeightMapGridMesh.h"

HeightMapImage::HeightMapImage(
	LPCTSTR pFileName,
	int nWidth, int nLength,
	XMFLOAT3 xmf3Scale)
	: m_nWidth{ nWidth },
	m_nLength{ nLength },
	m_xmf3Scale{ xmf3Scale }
{
	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(
		pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, m_nWidth* m_nLength, &dwBytesRead, NULL);
	::CloseHandle(hFile);

	// 이미지의 y축과 지형의 z축이 반대이므로 이미지를 상하반전시켜 저장
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++) {
		for (int x = 0; x < m_nWidth; x++) {
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] =
				pHeightMapPixels[x + (y * m_nWidth)];
		}
	}
	if (pHeightMapPixels)
		delete[] pHeightMapPixels;
}

HeightMapImage::~HeightMapImage()
{
	if (m_pHeightMapPixels)
		delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = nullptr;
}

float HeightMapImage::GetHeight(const float& fx, const float& fz) const
{
	if ((fx < 0.0f) ||
		(fz < 0.0f) ||
		(fx >= m_nWidth) ||
		(fz >= m_nLength))
		return 0.0f;

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];

	bool bRightToLeft = ((z % 2) & 1);
	if (bRightToLeft) {
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft - (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else {
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}

	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;

	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return fHeight * m_xmf3Scale.y;
}

XMFLOAT3 HeightMapImage::GetHeightMapNormal(
	const int& x, const int& z) const
{
	if ((x < 0.0f) ||
		(z < 0.0f) ||
		(x >= m_nWidth) ||
		(z >= m_nLength))
		return XMFLOAT3(0.0f, 1.0f, 0.0f);

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;

	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);

	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return xmf3Normal;
}

// HeightMapGridMesh --------------------------------------------------------------------------------------

HeightMapGridMesh::HeightMapGridMesh(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	int xStart, int zStart, int nWidth, int nLength,
	XMFLOAT3 xmf3Scale, void* pContext)
	: m_nWidth{ nWidth }, m_nLength{ nLength },
	m_xmf3Scale{ xmf3Scale }
{
	m_nVertices = m_nWidth * m_nLength;
	m_vxmf3Positions.resize(m_nVertices);
	m_vxmf3Normals.resize(m_nVertices);
	m_vxmf2TexCoords0.resize(m_nVertices);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	HeightMapImage* pHeightMapImage = (HeightMapImage*)pContext;
	int cxHeightMap = pHeightMapImage->GetHeightMapWidth();
	int czHeightMap = pHeightMapImage->GetHeightMapLength();

	float fHeight = 0.0f;
	for (int i = 0, z = zStart; z < (zStart + m_nLength); z++) {
		for (int x = xStart; x < (xStart + m_nWidth); x++, i++) {
			fHeight = GetHeight(x, z, pContext);
			XMFLOAT3 xmf3Position = XMFLOAT3(
				(x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			XMFLOAT3 xmf3Normal = GetNormal(x, z, pContext);
			XMFLOAT2 xmf2TexCoord0 =
				XMFLOAT2((float)x / m_nWidth, (float)z / m_nLength);

			m_vxmf3Positions[i] = xmf3Position;
			m_vxmf3Normals[i] = xmf3Normal;
			m_vxmf2TexCoords0[i] = xmf2TexCoord0;
		}
	}

	unique_ptr<VertexBuffer> pPositionBuffer = make_unique<VertexBuffer>();
	pPositionBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT3), m_nVertices, m_vxmf3Positions.data());
	m_umpVertexBuffers["position"] = move(pPositionBuffer);

	unique_ptr<VertexBuffer> pNormalBuffer = make_unique<VertexBuffer>();
	pNormalBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT3), m_nVertices, m_vxmf3Normals.data());
	m_umpVertexBuffers["normal"] = move(pNormalBuffer);

	unique_ptr<VertexBuffer> pTexCoordBuffer = make_unique<VertexBuffer>();
	pTexCoordBuffer->Create(pd3dDevice, pd3dCommandList, sizeof(XMFLOAT2), m_nVertices, m_vxmf2TexCoords0.data());
	m_umpVertexBuffers["uv0"] = move(pTexCoordBuffer);

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	m_vIndices.resize(m_nIndices);

	for (int i = 0, z = 0; z < nLength - 1; z++) {
		if ((z % 2) == 0) {
			for (int x = 0; x < nWidth; x++) {
				if ((x == 0) && (z > 0))
					m_vIndices[i++] = (UINT)(x + (z * nWidth));
				m_vIndices[i++] = (UINT)(x + (z * nWidth));
				m_vIndices[i++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else {
			for (int x = nWidth - 1; x >= 0; x--) {
				if (x == (nWidth - 1))
					m_vIndices[i++] = (UINT)(x + (z * nWidth));
				m_vIndices[i++] = (UINT)(x + (z * nWidth));
				m_vIndices[i++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		m_vIndices.data(), sizeof(UINT) * m_nIndices,
		m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

HeightMapGridMesh::~HeightMapGridMesh()
{
}

float HeightMapGridMesh::GetHeight(int x, int z, void* pContext)
{
	HeightMapImage* pHeightMapImage = (HeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z * nWidth)] * m_xmf3Scale.y;

	return fHeight;
}

XMFLOAT3 HeightMapGridMesh::GetNormal(int x, int z, void* pContext)
{
	HeightMapImage* pHeightMapImage = (HeightMapImage*)pContext;
	return pHeightMapImage->GetHeightMapNormal(x, z);
}

XMFLOAT4 HeightMapGridMesh::GetColor(int x, int z, void* pContext)
{
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);

	HeightMapImage* pHeightMapImage = (HeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();

	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);

	float fScale = Vector3::DotProduct(
		pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(
		pHeightMapImage->GetHeightMapNormal(x + 1, z),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(
		pHeightMapImage->GetHeightMapNormal(x + 1, z + 1),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(
		pHeightMapImage->GetHeightMapNormal(x, z + 1),
		xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f)
		fScale = 1.0f;
	if (fScale < 0.25f)
		fScale = 0.25f;

	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);

	return xmf4Color;
}