#include "pch.h"
#include "OpaqueShader.h"

D3D12_INPUT_LAYOUT_DESC OpaqueShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs =
		new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = {
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = {
		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = {
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return d3dInputLayoutDesc;
};

D3D12_SHADER_BYTECODE OpaqueShader::CreateVertexShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Default.hlsl", "VS", "vs_5_1", pd3dShaderBlob);
};

D3D12_SHADER_BYTECODE OpaqueShader::CreatePixelShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Default.hlsl", "PS", "ps_5_1", pd3dShaderBlob);
};