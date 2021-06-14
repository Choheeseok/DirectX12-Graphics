#pragma once
#include "Shader.h"
#include "TerrainObject.h"

class TerrainShader : public Shader
{
private:
	TerrainObject* m_pTerrain;
public:
	TerrainShader(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature,
		ID3D12DescriptorHeap* pd3dDescriptorheap,
		UINT nCbvSrvUavDescriptorIncrementSize)
		: Shader{
		pd3dDevice,
		pd3dCommandList,
		pd3dGraphicsRootSignature,
		pd3dDescriptorheap,
		nCbvSrvUavDescriptorIncrementSize },
		m_pTerrain{ nullptr } {};
	virtual ~TerrainShader() {};

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;

	D3D12_SHADER_BYTECODE CreateVertexShader(
		ID3DBlob** pd3dShaderBlob) override;
	D3D12_SHADER_BYTECODE CreatePixelShader(
		ID3DBlob** pd3dShaderBlob) override;

	TerrainObject* BuildTerrain(
		unordered_map<string, unique_ptr<Mesh>>& umMeshes,
		unordered_map<string, unique_ptr<Material>>& umMaterials) override;
};
