#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(int nMeshes, int nMaterials)
	: m_xmf3Position{ XMFLOAT3(0,0,0) },
	m_xmf3Scale{ XMFLOAT3(1,1,1) },
	m_xmf3Rotation{ XMFLOAT3(0,0,0) },
	m_xmf4x4World{ Matrix4x4::Identity() }
{
	m_vMeshes.resize(nMeshes);
	m_vMaterials.resize(nMaterials);
};

void GameObject::SetMesh(int nIndex, Mesh* pMesh)
{
	if (nIndex < m_vMeshes.size())
		m_vMeshes[nIndex] = pMesh;
}

void GameObject::SetMaterial(int nIndex, Material* pMaterial)
{
	if (nIndex < m_vMaterials.size())
		m_vMaterials[nIndex] = pMaterial;
}

void GameObject::Render(
	ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12DescriptorHeap* pd3dDescriptorHeap,
	UINT nCbvSrvUavDescriptorIncrementSize,
	UINT nInstances)
{
	for (int i = 0; i < m_vMeshes.size(); i++) {
		if (i < m_vMaterials.size() && m_vMaterials[i]) {
			m_vMaterials[i]->SetShaderVariables(
				pd3dCommandList, pd3dDescriptorHeap, nCbvSrvUavDescriptorIncrementSize);
		}
		m_vMeshes[i]->Render(pd3dCommandList, nInstances);
	}
}

XMFLOAT3 GameObject::GetRight() const
{
	XMFLOAT3 xmf3Right{
		m_xmf4x4World(0, 0),
		m_xmf4x4World(0, 1),
		m_xmf4x4World(0, 2) };
	return Vector3::Normalize(xmf3Right);
};

XMFLOAT3 GameObject::GetUp() const
{
	XMFLOAT3 xmf3Up{
		m_xmf4x4World(1, 0),
		m_xmf4x4World(1, 1),
		m_xmf4x4World(1, 2) };
	return Vector3::Normalize(xmf3Up);
};

XMFLOAT3 GameObject::GetLook() const
{
	XMFLOAT3 xmf3Look{
		m_xmf4x4World(2, 0),
		m_xmf4x4World(2, 1),
		m_xmf4x4World(2, 2) };
	return Vector3::Normalize(xmf3Look);
};

XMFLOAT3 GameObject::GetPosition() const
{
	XMFLOAT3 xmf3Position{
		m_xmf4x4World(3, 0),
		m_xmf4x4World(3, 1),
		m_xmf4x4World(3, 2) };
	return xmf3Position;
};

XMFLOAT4X4 GameObject::GetWorldMatrix() const
{
	return m_xmf4x4World;
}

void GameObject::SetWorldMatrix(XMFLOAT4X4 xmf4x4World)
{
	m_xmf4x4World = xmf4x4World;
}

void GameObject::SetPosition(const float& x, const float& y, const float& z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
	UpdateTransform();
}

void GameObject::SetPosition(const XMFLOAT3& xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void GameObject::SetScale(const float& x, const float& y, const float& z)
{
	m_xmf3Scale = XMFLOAT3(x, y, z);
	UpdateTransform();
}

void GameObject::SetScale(const XMFLOAT3& xmf3Scale)
{
	SetScale(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
}

void GameObject::SetRotation(const float& x, const float& y, const float& z)
{
	m_xmf3Rotation = XMFLOAT3(x, y, z);
	UpdateTransform();
}

void GameObject::SetRotation(const XMFLOAT3& xmf3Euler)
{
	SetRotation(xmf3Euler.x, xmf3Euler.y, xmf3Euler.z);
}

void GameObject::UpdateTransform()
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4World._11 = m_xmf3Scale.x;
	m_xmf4x4World._11 = m_xmf3Scale.y;
	m_xmf4x4World._11 = m_xmf3Scale.z;

	XMVECTOR xmvQuaternion =
		XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(m_xmf3Rotation.x),
			XMConvertToRadians(m_xmf3Rotation.y),
			XMConvertToRadians(m_xmf3Rotation.z));
	XMMATRIX xmmtxRotation = XMMatrixRotationQuaternion(xmvQuaternion);
	m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, xmmtxRotation);

	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}

void GameObject::MoveStraight(const float& fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	SetPosition(xmf3Position);
}

void GameObject::MoveStrafe(const float& fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	SetPosition(xmf3Position);
}