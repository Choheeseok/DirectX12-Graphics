#include "pch.h"
#include "BillboardShader.h"

D3D12_SHADER_BYTECODE BillboardShader::CreateVertexShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Billboard.hlsl", "VS", "vs_5_1", pd3dShaderBlob);
};

D3D12_SHADER_BYTECODE BillboardShader::CreateGeometryShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Billboard.hlsl", "GS", "gs_5_1", pd3dShaderBlob);
};

D3D12_SHADER_BYTECODE BillboardShader::CreatePixelShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Billboard.hlsl", "PS", "ps_5_1", pd3dShaderBlob);
};

D3D12_INPUT_LAYOUT_DESC BillboardShader::CreateInputLayout()
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
	// Vertex : m_xmf2TexCoord0 => Scale
	pd3dInputElementDescs[2] = {
		"SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return d3dInputLayoutDesc;
};