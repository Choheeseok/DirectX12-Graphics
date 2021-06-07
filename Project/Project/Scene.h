#pragma once

#include "Meshes.h"
#include "Materials.h"
#include "Camera.h"
#include "Shaders.h"
#include "Light.h"
#include "Timer.h"

class Scene
{
protected:
	ID3D12Device* m_pd3dDevice = nullptr;
	ID3D12GraphicsCommandList* m_pd3dCommandList = nullptr;

	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature;

	int m_nClientWidth;
	int m_nClientHeight;

	UINT m_nCbvSrvUavDescriptorIncrementSize;

	GameTimer& m_gameTimer;

	ComPtr<ID3D12DescriptorHeap> m_pd3dDescriptorHeap;

	ComPtr<ID3D12Resource> m_pInstanceMaterials;
	INSTANCE_MATERIAL_INFO* m_pInstanceMappedMaterials;

	ComPtr<ID3D12Resource> m_pLights;
	CB_LIGHT_INFO* m_pMappedLights;

	unordered_map<string, unique_ptr<Camera>> m_umCameras;
	unordered_map<string, unique_ptr<Shader>> m_umShaders;
	unordered_map<string, unique_ptr<Mesh>> m_umMeshes;
	unordered_map<string, unique_ptr<Texture>> m_umTextures;
	unordered_map<string, unique_ptr<Material>> m_umMaterials;
	unordered_map<string, unique_ptr<Light>> m_umLights;

	POINT m_pntLastMousePos;

public:
	Scene(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		GameTimer& gameTimer,
		const int& fClientWidth,
		const int& fClientHeight,
		const UINT& nDescriptorIncrementSize)
		: m_pd3dDevice{ pd3dDevice },
		m_pd3dCommandList{ pd3dCommandList },
		m_gameTimer{ gameTimer },
		m_nClientWidth{ fClientWidth },
		m_nClientHeight{ fClientHeight },
		m_nCbvSrvUavDescriptorIncrementSize{ nDescriptorIncrementSize }{};
	virtual ~Scene() {};

	virtual void OnMouseDown(WPARAM wBtnState, int x, int y) {};
	virtual void OnMouseUp(WPARAM wBtnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM wBtnState, int x, int y) {};

	virtual void CreateShaderVariables() {};
	virtual void UpdateShaderVariables() {};
	virtual void SetShaderVariables() {};

	virtual void BuildObjects() {};
	virtual void Update() {};
	virtual void Render() {};

	virtual void CreateGraphicsRootSignature() {};
};
