#include "pch.h"
#include "TerrainShader.h"

D3D12_INPUT_LAYOUT_DESC TerrainShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE TerrainShader::CreateVertexShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Terrain.hlsl", "VS_Terrain", "vs_5_1", pd3dShaderBlob);
};

D3D12_SHADER_BYTECODE TerrainShader::CreatePixelShader(
	ID3DBlob** pd3dShaderBlob)
{
	return CompileShaderFromFile(L"Terrain.hlsl", "PS_Terrain", "ps_5_1", pd3dShaderBlob);
};

TerrainObject* TerrainShader::BuildTerrain(
	unordered_map<string, unique_ptr<Mesh>>& umMeshes,
	unordered_map<string, unique_ptr<Material>>& umMaterials)
{
	m_d3dPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Shader::BuildObjects();

	XMFLOAT3 xmf3Scale(1.0f, 1.0f, 1.0f);

	m_pTerrain = new TerrainObject(
		m_pd3dDevice, m_pd3dCommandList, m_pd3dGraphicsRootSignature,
		L"Textures/HeightMap.raw", 257, 257, 17, 17, xmf3Scale);
	m_pTerrain->SetMaterial(0, umMaterials["Base_Texture"].get());
	m_pTerrain->SetPosition(0.0f, -120.0f, 0.0f);

	m_vGameObjects.emplace_back(m_pTerrain);

	CreateShaderVariables();

	return m_pTerrain;
}