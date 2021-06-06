#include "pch.h"
#include "Camera.h"
#include "d3dUtil.h"

Camera::Camera()
	:
	m_xmf3Position{ 0.0f, 0.0f ,0.0f },
	m_xmf3Right{ 1.0f ,0.0f ,0.0f },
	m_xmf3Up{ 0.0f ,1.0f ,0.0f },
	m_xmf3Look{ 0.0f ,0.0f ,1.0f },
	m_xmf4x4View{ Matrix4x4::Identity() },
	m_xmf4x4Projection{ Matrix4x4::Identity() },
	m_bfFrustum{},
	m_d3dViewport{ 0,0,FRAME_BUFFER_WIDTH,FRAME_BUFFER_HEIGHT },
	m_d3dScissorRect{ 0,0,FRAME_BUFFER_WIDTH,FRAME_BUFFER_HEIGHT },
	m_pPlayer{ nullptr },
	m_pcbMappedCamera{ }
{
}

Camera::~Camera()
{
}

void Camera::CreateShaderVariables(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(CB_CAMERA_INFO));
	m_pcbCamera = CreateBufferResource(pd3dDevice, pd3dCommandList,
		nullptr, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_pcbCamera->Map(0, nullptr, (void**)&m_pcbMappedCamera);
}

void Camera::UpdateShaderVariables(
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4View,
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4Projection,
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	XMStoreFloat3(&m_pcbMappedCamera->m_xmf3Position,
		XMLoadFloat3(&m_xmf3Position));
}

void Camera::SetShaderVariables(
	ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);

	pd3dCommandList->SetGraphicsRootConstantBufferView(
		0, m_pcbCamera->GetGPUVirtualAddress());
}

void Camera::SetFrustum(
	const float& fFov,
	const float& fAspect,
	const float& fNearZ,
	const float& fFarZ)
{
	m_fFovAngle = fFov;
	m_fAspectRatio = fAspect;
	m_fNearZ = fNearZ;
	m_fFarZ = fFarZ;
}

void Camera::SetViewport(
	const int& xTopLeft, const int& yTopLeft,
	const int& nWidth, const  int& nHeight,
	const float& fMinZ, const float& fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void Camera::SetScissorRect(
	const LONG& xLeft, const LONG& yTop,
	const  LONG& xRight, const LONG& yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR up = XMLoadFloat3(&m_xmf3Up);
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR pos = XMLoadFloat3(&m_xmf3Position);

	look = XMVector3Normalize(look);
	up = XMVector3Normalize(XMVector3Cross(look, right));
	right = XMVector3Cross(up, look);

	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, look));

	XMStoreFloat3(&m_xmf3Right, right);
	XMStoreFloat3(&m_xmf3Up, up);
	XMStoreFloat3(&m_xmf3Look, look);

	m_xmf4x4View(0, 0) = m_xmf3Right.x;
	m_xmf4x4View(1, 0) = m_xmf3Right.y;
	m_xmf4x4View(2, 0) = m_xmf3Right.z;
	m_xmf4x4View(3, 0) = x;

	m_xmf4x4View(0, 1) = m_xmf3Up.x;
	m_xmf4x4View(1, 1) = m_xmf3Up.y;
	m_xmf4x4View(2, 1) = m_xmf3Up.z;
	m_xmf4x4View(3, 1) = y;

	m_xmf4x4View(0, 2) = m_xmf3Look.x;
	m_xmf4x4View(1, 2) = m_xmf3Look.y;
	m_xmf4x4View(2, 2) = m_xmf3Look.z;
	m_xmf4x4View(3, 2) = z;

	m_xmf4x4View(0, 3) = 0.0f;
	m_xmf4x4View(1, 3) = 0.0f;
	m_xmf4x4View(2, 3) = 0.0f;
	m_xmf4x4View(3, 3) = 1.0f;
}

void Camera::UpdateProjectionMatrix()
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovAngle), m_fAspectRatio, m_fNearZ, m_fFarZ);

	XMStoreFloat4x4(&m_xmf4x4Projection, proj);
	BoundingFrustum::CreateFromMatrix(m_bfFrustum, proj);
}

void Camera::MoveStraight(const float& fDistance)
{
	XMFLOAT3 xmf3Position = m_xmf3Position;
	XMFLOAT3 xmf3Look = m_xmf3Look;
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	m_xmf3Position = xmf3Position;

	UpdateViewMatrix();
}

void Camera::MoveStrafe(const float& fDistance)
{
	XMFLOAT3 xmf3Position = m_xmf3Position;
	XMFLOAT3 xmf3Right = m_xmf3Right;
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	m_xmf3Position = xmf3Position;

	UpdateViewMatrix();
}

void Camera::Rotate(const float& x, const float& y, const float& z)
{
	if (false == IsZero(x)) {
		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), x);
		XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));
	}
	if (false == IsZero(y)) {
		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), y);
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), R));
		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));
	}
	if (false == IsZero(z)) {
		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), z);
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), R));
		XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	}

	UpdateViewMatrix();
}