#pragma once

#include "Mesh.h"

class BillboardMesh : public Mesh
{
public:
	BillboardMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList);
	~BillboardMesh() {};
};
