#pragma once
#include "Shader.h"

class SkyBoxShader : public Shader
{
public:
	SkyBoxShader(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature,
		ID3D12DescriptorHeap* pd3dDescriptorheap,
		UINT nCbvSrvUavDescriptorIncrementSize)
		: Shader{ pd3dDevice,
		pd3dCommandList,
		pd3dGraphicsRootSignature,
		pd3dDescriptorheap,
		nCbvSrvUavDescriptorIncrementSize } {};
	virtual ~SkyBoxShader() {};

	D3D12_SHADER_BYTECODE CreateVertexShader(
		ID3DBlob** pd3dShaderBlob) override;
	D3D12_SHADER_BYTECODE CreatePixelShader(
		ID3DBlob** pd3dShaderBlob) override;

	D3D12_RASTERIZER_DESC CreateRasterizerState() override;
	D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState() override;
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;

	void BuildObjects(
		unordered_map<string, unique_ptr<Mesh>>& umMeshes,
		unordered_map<string, unique_ptr<Material>>& umMaterials)override;
	void Render() override;
};
