#pragma once

#include "Mesh.h"

class CubeMesh : public Mesh
{
public:
	CubeMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList);
	~CubeMesh() {};
};