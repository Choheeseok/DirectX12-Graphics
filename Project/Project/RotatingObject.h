#pragma once

#include "GameObject.h"

class RotatingObject : public GameObject
{
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	RotatingObject(int nMeshes, int nMaterials)
		: GameObject{ nMeshes, nMaterials },
		m_xmf3RotationAxis{ Vector3::Normalize(Vector3::Random()) },
		m_fRotationSpeed{ RandomFloat() * 90.0f }{};
	virtual ~RotatingObject() {};

	void Animate(const float& fTimeElapsed) override;

	void Rotate(const float& fTimeElapsed);
};
