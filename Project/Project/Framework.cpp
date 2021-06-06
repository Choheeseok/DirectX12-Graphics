#include "pch.h"
#include "Framework.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try {
		Framework app(hInstance);
		if (!app.Initialize())
			return 0;

		return app.Run();
	}
	catch (DxException& e) {
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}

Framework::Framework(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

Framework::~Framework()
{
}

bool Framework::Initialize()
{
	if (false == D3DApp::Initialize())
		return false;
	m_umScenes["Main"] = make_unique<MainScene>(
		md3dDevice.Get(), mCommandList.Get(), mGameTimer, mClientWidth, mClientHeight, mCbvSrvUavDescriptorSize);
	m_pCurrentScene = m_umScenes["Main"].get();

	ThrowIfFailed(mCommandList->Reset(mCommandAllocator.Get(), nullptr));
	m_pCurrentScene->BuildObjects();

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* pd3dCommandLists[]{ mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(
		_countof(pd3dCommandLists), pd3dCommandLists);

	WaitForGpuComplete();

	return true;
}

void Framework::OnResize()
{
	D3DApp::OnResize();
}

void Framework::Update(const GameTimer& gameTimer)
{
	m_pCurrentScene->Update();
}

void Framework::Draw(const GameTimer& gameTimer)
{
	ThrowIfFailed(mCommandAllocator->Reset());
	ThrowIfFailed(mCommandList->Reset(mCommandAllocator.Get(), nullptr));

	mCommandList->ResourceBarrier(
		1, &CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		));

	mCommandList->ClearRenderTargetView(
		CurrentBackBufferView(), Colors::DarkGray, 0, nullptr);
	mCommandList->ClearDepthStencilView(
		DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(
		1, &CurrentBackBufferView(), TRUE, &DepthStencilView());

	m_pCurrentScene->Render();

	mCommandList->ResourceBarrier(
		1, &CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		));

	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* pd3dCommandList[]{ mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(
		_countof(pd3dCommandList), pd3dCommandList);

	WaitForGpuComplete();

	mSwapChain->Present(0, 0);

	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % SwapChainBufferCount;
}

void Framework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++mCurrentFence;
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), nFenceValue));
	if (mFence->GetCompletedValue() < nFenceValue) {
		ThrowIfFailed(mFence->SetEventOnCompletion(nFenceValue, mFenceEvent));
		::WaitForSingleObject(mFenceEvent, INFINITE);
	}
}

void Framework::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_pCurrentScene->OnMouseDown(btnState, x, y);
	SetCapture(MainWnd());
};
void Framework::OnMouseUp(WPARAM btnState, int x, int y)
{
	m_pCurrentScene->OnMouseUp(btnState, x, y);
	ReleaseCapture();
};

void Framework::OnMouseMove(WPARAM btnState, int x, int y)
{
	m_pCurrentScene->OnMouseMove(btnState, x, y);
}