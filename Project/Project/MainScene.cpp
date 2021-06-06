#include "pch.h"
#include "MainScene.h"

void MainScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_pntLastMousePos.x = x;
	m_pntLastMousePos.y = y;
};

void MainScene::OnMouseUp(WPARAM btnState, int x, int y)
{
};

void MainScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (0 != (btnState & MK_LBUTTON)) {
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_pntLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_pntLastMousePos.y));

		m_umCameras["Main"]->Rotate(dy, dx, 0.0f);
	}

	m_pntLastMousePos.x = x;
	m_pntLastMousePos.y = y;
};

void MainScene::OnKeyboardInput()
{
	const float deltaTime{ m_gameTimer.GetTimeElapsed() };

	if (GetAsyncKeyState('W') & 0x8000)
		m_umCameras["Main"]->MoveStraight(20.0f * deltaTime);

	if (GetAsyncKeyState('S') & 0x8000)
		m_umCameras["Main"]->MoveStraight(-20.0f * deltaTime);

	if (GetAsyncKeyState('A') & 0x8000)
		m_umCameras["Main"]->MoveStrafe(-20.0f * deltaTime);

	if (GetAsyncKeyState('D') & 0x8000)
		m_umCameras["Main"]->MoveStrafe(20.0f * deltaTime);
}

void MainScene::BuildObjects()
{
	CreateGraphicsRootSignature();

	BuildCameras();
	BuildMeshes();
	BuildShaders();

	CreateShaderVariables();

	Camera* pMainCamera = m_umCameras["Main"].get();

	pMainCamera->SetFrustum(
		60.0f,
		static_cast<float>(m_nClientWidth) / (m_nClientHeight),
		1.01f, 5000.0f);
	pMainCamera->UpdateProjectionMatrix();

	pMainCamera->SetViewport(
		0, 0, m_nClientWidth, m_nClientHeight, 0.0f, 1.0f);
	pMainCamera->SetScissorRect(
		0, 0, m_nClientWidth, m_nClientHeight);
}

void MainScene::Update()
{
	OnKeyboardInput();

	for (const auto& shader : m_umShaders)
		shader.second->AnimateObjects(m_gameTimer.GetTimeElapsed());
}

void MainScene::Render()
{
	m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());

	m_umCameras["Main"]->UpdateShaderVariables(m_pd3dCommandList);
	m_umCameras["Main"]->SetShaderVariables(m_pd3dCommandList);

	UpdateShaderVariables();
	SetShaderVariables();

	for (const auto& shader : m_umShaders) {
		shader.second->Render();
	}
}

void MainScene::CreateGraphicsRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE d3dDescriptorTable[1];
	d3dDescriptorTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].InitAsConstantBufferView(0); // b0 Camera
	pd3dRootParameters[1].InitAsShaderResourceView(0); // t0 Object

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	d3dSamplerDesc.Init(0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f, 1,
		D3D12_COMPARISON_FUNC_ALWAYS,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0.0f, D3D12_FLOAT32_MAX,
		D3D12_SHADER_VISIBILITY_PIXEL, 0);

	CD3DX12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	d3dRootSignatureDesc.Init(
		_countof(pd3dRootParameters),
		pd3dRootParameters,
		1, &d3dSamplerDesc,
		d3dRootSignatureFlags);

	ComPtr<ID3DBlob> pd3dSignatureBlob;
	ComPtr<ID3DBlob> pd3dErrorBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(
		&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		pd3dSignatureBlob.GetAddressOf(), pd3dErrorBlob.GetAddressOf()));

	ThrowIfFailed(m_pd3dDevice->CreateRootSignature(
		0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_pd3dGraphicsRootSignature.GetAddressOf())));
}

void MainScene::BuildCameras()
{
	m_umCameras["Main"] =
		make_unique<Camera>();
	m_umCameras["Main"]->CreateShaderVariables(
		m_pd3dDevice, m_pd3dCommandList);
}

void MainScene::BuildMeshes()
{
	m_umMeshes["Cube"] =
		make_unique<CubeMesh>(m_pd3dDevice, m_pd3dCommandList);

	m_umMeshes["Sphere"] =
		make_unique<SphereMesh>(m_pd3dDevice, m_pd3dCommandList);
}

void MainScene::BuildShaders()
{
	m_umShaders["Opaque"] =
		make_unique<OpaqueShader>(
			m_pd3dDevice,
			m_pd3dCommandList,
			m_pd3dGraphicsRootSignature.Get(),
			nullptr,
			m_nCbvSrvUavDescriptorIncrementSize);
	m_umShaders["Opaque"]->BuildObjects(m_umMeshes);
}