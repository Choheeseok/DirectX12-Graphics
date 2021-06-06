#include "pch.h"
#include "d3dUtil.h"
#include "Shader.h"

D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(
	const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> errors;

	HRESULT hr = ::D3DCompileFromFile(
		pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, errors.GetAddressOf());

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

Shader::~Shader()
{
	for (GameObject* obj : m_vGameObjects) {
		delete obj;
	}
	m_vGameObjects.erase(m_vGameObjects.begin(), m_vGameObjects.end());
}

void Shader::CreatePipelineStates()
{
	ComPtr<ID3DBlob> pd3dVertexShaderBlob;
	ComPtr<ID3DBlob> pd3dHullShaderBlob;
	ComPtr<ID3DBlob> pd3dDomainShaderBlob;
	ComPtr<ID3DBlob> pd3dGeometryShaderBlob;
	ComPtr<ID3DBlob> pd3dPixelShaderBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(pd3dVertexShaderBlob.GetAddressOf());
	d3dPipelineStateDesc.HS = CreateHullShader(pd3dHullShaderBlob.GetAddressOf());
	d3dPipelineStateDesc.DS = CreateDomainShader(pd3dDomainShaderBlob.GetAddressOf());
	d3dPipelineStateDesc.GS = CreateGeometryShader(pd3dGeometryShaderBlob.GetAddressOf());
	d3dPipelineStateDesc.PS = CreatePixelShader(pd3dPixelShaderBlob.GetAddressOf());
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = m_d3dPrimitiveTopologyType;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	ThrowIfFailed(
		m_pd3dDevice->CreateGraphicsPipelineState(
			&d3dPipelineStateDesc, IID_PPV_ARGS(&m_pd3dPipelineState)));

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob** pd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return d3dShaderByteCode;
}

D3D12_SHADER_BYTECODE Shader::CreateHullShader(ID3DBlob** pd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return d3dShaderByteCode;
}

D3D12_SHADER_BYTECODE Shader::CreateDomainShader(ID3DBlob** pd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return d3dShaderByteCode;
}

D3D12_SHADER_BYTECODE Shader::CreateGeometryShader(ID3DBlob** pd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return d3dShaderByteCode;
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob** pd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return d3dShaderByteCode;
}

D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc{};
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc{};
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return d3dRasterizerDesc;
}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc{};
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return d3dDepthStencilDesc;
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{};
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return d3dInputLayoutDesc;
}

void Shader::CreateShaderVariables()
{
	UINT ncbElementBytes =
		d3dUtil::CalcConstantBufferByteSize(
			sizeof(INSTANCE_GAMEOBJECT_INFO) * m_vGameObjects.size());
	m_pInstanceGameObjects = CreateBufferResource(m_pd3dDevice, m_pd3dCommandList,
		nullptr, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	m_pInstanceGameObjects->Map(0, nullptr, (void**)&m_pInstanceMappedGameObjects);
}

void Shader::UpdateShaderVariables()
{
	for (int i = 0; i < m_vGameObjects.size(); i++) {
		XMStoreFloat4x4(&m_pInstanceMappedGameObjects[i].m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(&m_vGameObjects[i]->GetWorldMatrix())));
	}
}

void Shader::SetShaderVariables()
{
	m_pd3dCommandList->SetGraphicsRootShaderResourceView(
		1, m_pInstanceGameObjects->GetGPUVirtualAddress());
}

void Shader::BuildObjects()
{
	CreatePipelineStates();
}

void Shader::AnimateObjects(const float& fTimeElapsed)
{
	for (GameObject* obj : m_vGameObjects)
		obj->Animate(fTimeElapsed);
}

void Shader::Render()
{
	m_pd3dCommandList->SetPipelineState(m_pd3dPipelineState.Get());

	UpdateShaderVariables();
	SetShaderVariables();
}