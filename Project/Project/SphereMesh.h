#pragma once

#include "Mesh.h"

class SphereMesh : public Mesh
{
public:
	SphereMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList);
	~SphereMesh() {};
};