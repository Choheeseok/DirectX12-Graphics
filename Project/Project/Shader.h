#pragma once

#include "GameObjects.h"

struct INSTANCE_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	UINT m_nMaterialIndex;
};

class Shader
{
protected:
	ID3D12Device* m_pd3dDevice;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature;

	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE m_d3dPrimitiveTopologyType;

	vector<GameObject*> m_vGameObjects;

	ComPtr<ID3D12Resource> m_pInstanceGameObjects;
	INSTANCE_GAMEOBJECT_INFO* m_pInstanceMappedGameObjects;

	ID3D12DescriptorHeap* m_pd3dDescriptorHeap;
	UINT m_nCbvSrvUavDescriptorIncrementSize;

public:
	D3D12_SHADER_BYTECODE CompileShaderFromFile(
		const WCHAR* pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	Shader(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature,
		ID3D12DescriptorHeap* pd3dDescriptorheap,
		UINT nCbvSrvUavDescriptorIncrementSize)
		: m_pd3dDevice{ pd3dDevice },
		m_pd3dCommandList{ pd3dCommandList },
		m_pd3dGraphicsRootSignature{ pd3dGraphicsRootSignature },
		m_pd3dDescriptorHeap{ pd3dDescriptorheap },
		m_nCbvSrvUavDescriptorIncrementSize{ nCbvSrvUavDescriptorIncrementSize },
		m_d3dPrimitiveTopologyType{ D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE },
		m_pInstanceMappedGameObjects{ nullptr }{};
	virtual ~Shader();

	virtual void CreatePipelineStates();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(
		ID3DBlob** pd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(
		ID3DBlob** pd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(
		ID3DBlob** pd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(
		ID3DBlob** pd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(
		ID3DBlob** pd3dShaderBlob);
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	void CreateShaderVariables();
	void UpdateShaderVariables();
	void SetShaderVariables();

	virtual void BuildObjects();

	virtual TerrainObject* BuildTerrain(
		unordered_map<string, unique_ptr<Mesh>>& umMeshes,
		unordered_map<string, unique_ptr<Material>>& umMaterials) {
		return nullptr;
	};

	void AddObject(GameObject* pObject);

	virtual void AnimateObjects(const float& fTimeElapsed);
	virtual void Render();
};
