#pragma once

#include "Meshes.h"
#include "Camera.h"
#include "Shaders.h"
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

	unordered_map<string, unique_ptr<Camera>> m_umCameras;
	unordered_map<string, unique_ptr<Shader>> m_umShaders;
	unordered_map<string, unique_ptr<Mesh>> m_umMeshes;

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

	void CreateShaderVariables();
	void UpdateShaderVariables();
	void SetShaderVariables();

	virtual void BuildObjects() {};
	virtual void Update() {};
	virtual void Render() {};

	virtual void CreateGraphicsRootSignature() {};
};
