#pragma once

#include "Scene.h"

class MainScene : public Scene
{
public:
	MainScene(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		GameTimer& gameTimer,
		const int& fClientWidth,
		const int& fClientHeight,
		const UINT& nDescriptorIncrementSize)
		: Scene{
		pd3dDevice ,
		pd3dCommandList ,
		gameTimer,
		fClientWidth ,
		fClientHeight,
		nDescriptorIncrementSize
	} {};
	~MainScene() {};

	void OnMouseDown(WPARAM wBtnState, int x, int y) override;
	void OnMouseUp(WPARAM wBtnState, int x, int y) override;
	void OnMouseMove(WPARAM wBtnState, int x, int y) override;

	void OnKeyboardInput();

	void BuildObjects() override;
	void Update() override;
	void Render() override;

	void CreateGraphicsRootSignature() override;
public:
	void BuildCameras();
	void BuildMeshes();
	void BuildShaders();
};