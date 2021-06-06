#pragma once

#include "d3dApp.h"
#include "Scenes.h"

class Framework : public D3DApp
{
private:
	unordered_map<string, unique_ptr<Scene>> m_umScenes;
	Scene* m_pCurrentScene;
public:
	Framework(HINSTANCE hInstance);
	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	virtual ~Framework();

	virtual bool Initialize() override;
protected:

	virtual void OnResize();
	virtual void Update(const GameTimer& gameTimer);
	virtual void Draw(const GameTimer& gameTimer);

	void WaitForGpuComplete();

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
};
