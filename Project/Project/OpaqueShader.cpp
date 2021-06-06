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
		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = {
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return d3dInputLayoutDesc;
};

D3D12_SHADER_BYTECODE OpaqueShader::CreateVertexShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", "VS", "vs_5_1", pd3dShaderBlob);
};

D3D12_SHADER_BYTECODE OpaqueShader::CreatePixelShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", "PS", "ps_5_1", pd3dShaderBlob);
};

void OpaqueShader::BuildObjects(
	unordered_map<string, unique_ptr<Mesh>>& umMeshes)
{
	Shader::BuildObjects();

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			GameObject* obj = new GameObject(1);
			obj->SetMesh(0, umMeshes["Sphere"].get());
			obj->SetPosition(10.0f * urd(dre) - 5, 10.0f * urd(dre) - 5, 10.0f * urd(dre) - 5);
			m_vGameObjects.emplace_back(obj);
		}
	}

	CreateShaderVariables();
}

void OpaqueShader::Render()
{
	Shader::Render();

	m_vGameObjects[0]->Render(
		m_pd3dCommandList,
		m_pd3dDescriptorHeap,
		m_nCbvSrvUavDescriptorIncrementSize,
		m_vGameObjects.size());
}