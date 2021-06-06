#pragma once

struct CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3 m_xmf3Position;
};

class Player;

class Camera
{
private:
	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Right;
	XMFLOAT3	m_xmf3Up;
	XMFLOAT3	m_xmf3Look;

	float m_fNearZ;
	float m_fFarZ;
	float m_fAspectRatio;
	float m_fFovAngle;

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;

	BoundingFrustum m_bfFrustum;

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	Player* m_pPlayer;

	ComPtr<ID3D12Resource> m_pcbCamera;
	CB_CAMERA_INFO* m_pcbMappedCamera;

public:
	Camera();
	~Camera();

	void CreateShaderVariables(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(
		ID3D12GraphicsCommandList* pd3dCommandList);
	void SetShaderVariables(
		ID3D12GraphicsCommandList* pd3dCommandList);

	void SetFrustum(
		const float& fFov,
		const float& fAspect,
		const float& fNearZ,
		const float& fFarZ);
	void SetViewport(
		const int& xTopLeft, const int& yTopLeft,
		const int& nWidth, const  int& nHeight,
		const float& fMinZ, const float& fMaxZ);
	void SetScissorRect(
		const LONG& xLeft, const LONG& yTop,
		const  LONG& xRight, const LONG& yBottom);

	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	void MoveStraight(const float& fDistance);
	void MoveStrafe(const float& fDistance);

	void Rotate(const float& x, const float& y, const float& z);
};
