#include "pch.h"
#include "RotatingObject.h"

void RotatingObject::Animate(const float& fTimeElapsed)
{
	Rotate(fTimeElapsed);
}

void RotatingObject::Rotate(const float& fTimeElapsed)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(
		XMLoadFloat3(&m_xmf3RotationAxis),
		XMConvertToRadians(m_fRotationSpeed * fTimeElapsed));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}