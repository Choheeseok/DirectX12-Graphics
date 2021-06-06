#pragma once

#include "Mesh.h"

class GameObject
{
protected:
	vector<Mesh*> m_vMeshes;

	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Scale;
	XMFLOAT3 m_xmf3Rotation;

	XMFLOAT4X4 m_xmf4x4World;
public:
	GameObject(int nMeshes);
	virtual ~GameObject() {};

	void SetMesh(int nIndex, Mesh* pMesh);

	virtual void Animate(const float& fTimeElapsed) {};
	virtual void Render(
		ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12DescriptorHeap* pd3dDescriptorHeap,
		UINT nCbvSrvUavDescriptorIncrementSize,
		UINT nInstances);

	XMFLOAT3 GetRight() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetPosition() const;
	XMFLOAT4X4 GetWorldMatrix() const;

	void SetWorldMatrix(XMFLOAT4X4 xmf4x4World);
	void SetPosition(const float& x, const float& y, const float& z);
	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetScale(const float& x, const float& y, const float& z);
	void SetScale(const XMFLOAT3& xmf3Scale);
	void SetRotation(const float& x, const float& y, const float& z);
	void SetRotation(const XMFLOAT3& xmf3Euler);
	void UpdateTransform();

	void MoveStraight(const float& fDistance);
	void MoveStrafe(const float& fDistance);
};
