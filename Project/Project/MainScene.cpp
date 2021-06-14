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

	m_umLights["Camera_Spot"]->m_xmf3Position = m_umCameras["Main"]->GetPosition();
	m_umLights["Camera_Spot"]->m_xmf3Direction = m_umCameras["Main"]->GetLook();
}

void MainScene::CreateShaderVariables()
{
	UINT ncbElementBytes =
		d3dUtil::CalcConstantBufferByteSize(
			sizeof(INSTANCE_MATERIAL_INFO) * m_umMaterials.size());
	m_pInstanceMaterials = CreateBufferResource(m_pd3dDevice, m_pd3dCommandList,
		nullptr, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	m_pInstanceMaterials->Map(0, nullptr, (void**)&m_pInstanceMappedMaterials);

	ncbElementBytes =
		d3dUtil::CalcConstantBufferByteSize(
			sizeof(CB_LIGHT_INFO));
	m_pLights = CreateBufferResource(m_pd3dDevice, m_pd3dCommandList,
		nullptr, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	m_pLights->Map(0, nullptr, (void**)&m_pMappedLights);
}

void MainScene::UpdateShaderVariables()
{
	for (const auto& camera : m_umCameras)
		camera.second->UpdateShaderVariables(m_pd3dCommandList);

	for (const auto& shader : m_umShaders)
		shader.second->UpdateShaderVariables();

	int i = 0;
	for (const auto& material : m_umMaterials) {
		XMStoreFloat4(&m_pInstanceMappedMaterials[i].m_xmf4Ambient,
			XMLoadFloat4(&material.second->m_xmf4Ambient));
		XMStoreFloat4(&m_pInstanceMappedMaterials[i].m_xmf4Diffuse,
			XMLoadFloat4(&material.second->m_xmf4Diffuse));
		XMStoreFloat4(&m_pInstanceMappedMaterials[i].m_xmf4Specular,
			XMLoadFloat4(&material.second->m_xmf4Specular));
		XMStoreFloat4(&m_pInstanceMappedMaterials[i].m_xmf4Emissive,
			XMLoadFloat4(&material.second->m_xmf4Emissive));
		i++;
	}

	i = 0;
	for (const auto& light : m_umLights) {
		memcpy(&m_pMappedLights->m_pLights[i++], light.second.get(), sizeof(Light));
	}
	m_pMappedLights->m_nLights = m_umLights.size();
}

void MainScene::SetShaderVariables()
{
	m_pd3dCommandList->SetGraphicsRootShaderResourceView(
		2, m_pInstanceMaterials->GetGPUVirtualAddress());

	m_pd3dCommandList->SetGraphicsRootConstantBufferView(
		4, m_pLights->GetGPUVirtualAddress());
}

void MainScene::BuildObjects()
{
	CreateGraphicsRootSignature();

	BuildCameras();
	BuildMeshes();
	BuildTextures();
	BuildDescriptorHeap();
	BuildDescriptors();
	BuildMaterials();
	BuildShaders();
	BuildLights();

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

	ID3D12DescriptorHeap* pd3dDescriptorheaps[] = { m_pd3dDescriptorHeap.Get() };
	m_pd3dCommandList->SetDescriptorHeaps(
		_countof(pd3dDescriptorheaps), pd3dDescriptorheaps);

	UpdateShaderVariables();
	SetShaderVariables();

	m_umShaders["Point"]->Render();
	m_umShaders["Terrain"]->Render();
	m_umShaders["Billboard"]->Render();
	m_umShaders["Opaque"]->Render();
	m_umShaders["SkyBox"]->Render();
}

void MainScene::CreateGraphicsRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE d3dDescriptorTable[2];
	d3dDescriptorTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0); // DiffuseMap
	d3dDescriptorTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0); // CubeMap

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[6];
	pd3dRootParameters[0].InitAsConstantBufferView(0); // b0 Camera
	pd3dRootParameters[1].InitAsShaderResourceView(0); // t0 Object
	pd3dRootParameters[2].InitAsShaderResourceView(1); // t1 Material
	pd3dRootParameters[3].InitAsDescriptorTable(1, &d3dDescriptorTable[0]); // t2 DiffuseMap
	pd3dRootParameters[4].InitAsConstantBufferView(1); // b1 Light
	pd3dRootParameters[5].InitAsDescriptorTable(1, &d3dDescriptorTable[1]); // t3 CubeMap

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
		D3D12_SHADER_VISIBILITY_ALL, 0);

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

void MainScene::BuildDescriptorHeap()
{
	ComPtr< ID3D12DescriptorHeap> pd3dTextureDescriptorHeap;
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc{};
	d3dDescriptorHeapDesc.NumDescriptors = m_umTextures.size() + 2; // Terrain position
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_pd3dDevice->CreateDescriptorHeap(
		&d3dDescriptorHeapDesc, IID_PPV_ARGS(&pd3dTextureDescriptorHeap)));
	m_pd3dDescriptorHeap = pd3dTextureDescriptorHeap;
}

void MainScene::BuildDescriptors()
{
	// 디스크립터들을 채운다
	CD3DX12_CPU_DESCRIPTOR_HANDLE d3dDescriptorHandle{
		m_pd3dDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dSrvDesc{};
	d3dSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	D3D12_UNORDERED_ACCESS_VIEW_DESC d3dUavDesc{};

	D3D12_RESOURCE_DESC d3dResourceDesc = {};

	int i = 0;
	for (auto& texture : m_umTextures) {
		for (int j = 0; j < texture.second->m_vTextures.size(); j++) {
			d3dResourceDesc =
				texture.second->m_vTextures[j]->GetDesc();
			d3dSrvDesc.Format = d3dResourceDesc.Format;
			switch (texture.second->m_nResourceType) {
			case Texture::RESOURCE_TEXTURE2D:
				// (d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
				// (d3dResourceDesc.DepthOrArraySize == 1)
			case Texture::RESOURCE_TEXTURE2D_ARRAY:
				// []
				d3dSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				d3dSrvDesc.Texture2D.MipLevels = -1;
				d3dSrvDesc.Texture2D.MostDetailedMip = 0;
				d3dSrvDesc.Texture2D.PlaneSlice = 0;
				d3dSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				break;
			case Texture::RESOURCE_TEXTURE2DARRAY:
				// (d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
				// (d3dResourceDesc.DepthOrArraySize != 1)
				d3dSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				d3dSrvDesc.Texture2DArray.MipLevels = -1;
				d3dSrvDesc.Texture2DArray.MostDetailedMip = 0;
				d3dSrvDesc.Texture2DArray.PlaneSlice = 0;
				d3dSrvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
				d3dSrvDesc.Texture2DArray.FirstArraySlice = 0;
				d3dSrvDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
				break;
			case Texture::RESOURCE_TEXTURE_CUBE:
				// (d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
				// (d3dResourceDesc.DepthOrArraySize == 6)
				d3dSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				d3dSrvDesc.TextureCube.MipLevels = 1;
				d3dSrvDesc.TextureCube.MostDetailedMip = 0;
				d3dSrvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
				break;
			case Texture::RESOURCE_BUFFER:
				// (d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
				d3dSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				d3dSrvDesc.Buffer.FirstElement = 0;
				d3dSrvDesc.Buffer.NumElements = 0;
				d3dSrvDesc.Buffer.StructureByteStride = 0;
				d3dSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				break;
			}

			m_pd3dDevice->CreateShaderResourceView(
				texture.second->m_vTextures[j].Get(), &d3dSrvDesc, d3dDescriptorHandle);

			texture.second->m_nSrvHeapIndex = i++;
			d3dDescriptorHandle.Offset(1, m_nCbvSrvUavDescriptorIncrementSize);
		}
	}
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

	m_umMeshes["Grass"] =
		make_unique<BillboardMesh>(m_pd3dDevice, m_pd3dCommandList);
}

void MainScene::BuildShaders()
{
	m_umShaders["Terrain"] =
		make_unique<TerrainShader>(
			m_pd3dDevice,
			m_pd3dCommandList,
			m_pd3dGraphicsRootSignature.Get(),
			m_pd3dDescriptorHeap.Get(),
			m_nCbvSrvUavDescriptorIncrementSize);
	m_pTerrain =
		m_umShaders["Terrain"]->BuildTerrain(m_umMeshes, m_umMaterials);
	m_umShaders["Terrain"]->CreateShaderVariables();

	m_umShaders["Opaque"] =
		make_unique<OpaqueShader>(
			m_pd3dDevice,
			m_pd3dCommandList,
			m_pd3dGraphicsRootSignature.Get(),
			m_pd3dDescriptorHeap.Get(),
			m_nCbvSrvUavDescriptorIncrementSize);
	m_umShaders["Opaque"]->BuildObjects();

	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 20; j++) {
			GameObject* obj = new GameObject(1, 1);
			obj->SetMesh(0, m_umMeshes["Sphere"].get());
			obj->SetMaterial(0, m_umMaterials["WoodCrate1"].get());

			float fScale = urd(dre) * 3.0f + 1.0f;
			float fx = urd(dre) * m_pTerrain->GetWidth();
			float fy = urd(dre) * 50.0f;
			float fz = urd(dre) * m_pTerrain->GetLength();
			obj->SetPosition(Vector3::Add(XMFLOAT3(
				fx, m_pTerrain->GetHeight(fx, fz) + fy, fz),
				m_pTerrain->GetPosition()));
			obj->SetScale(XMFLOAT3(fScale, fScale, fScale));

			m_umShaders["Opaque"]->AddObject(obj);
		}
	}
	m_umShaders["Opaque"]->CreateShaderVariables();

	m_umShaders["Point"] =
		make_unique<OpaqueShader>(
			m_pd3dDevice,
			m_pd3dCommandList,
			m_pd3dGraphicsRootSignature.Get(),
			m_pd3dDescriptorHeap.Get(),
			m_nCbvSrvUavDescriptorIncrementSize);
	m_umShaders["Point"]->BuildObjects();

	GameObject* light = new GameObject(1, 1);
	light->SetMesh(0, m_umMeshes["Cube"].get());
	light->SetMaterial(0, m_umMaterials["WoodCrate2"].get());
	light->SetPosition(Vector3::Add(
		XMFLOAT3(40.0f, m_pTerrain->GetHeight(40.0f, 40.0f) + 20.0f, 40.0f),
		m_pTerrain->GetPosition()));
	m_umShaders["Point"]->AddObject(light);
	light = new GameObject(1, 1);
	light->SetMesh(0, m_umMeshes["Cube"].get());
	light->SetMaterial(0, m_umMaterials["WoodCrate2"].get());
	light->SetPosition(Vector3::Add(
		XMFLOAT3(70.0f, m_pTerrain->GetHeight(70.0f, 70.0f) + 50.0f, 70.0f),
		m_pTerrain->GetPosition()));
	m_umShaders["Point"]->AddObject(light);
	m_umShaders["Point"]->CreateShaderVariables();

	m_umShaders["SkyBox"] =
		make_unique<SkyBoxShader>(
			m_pd3dDevice,
			m_pd3dCommandList,
			m_pd3dGraphicsRootSignature.Get(),
			m_pd3dDescriptorHeap.Get(),
			m_nCbvSrvUavDescriptorIncrementSize);
	m_umShaders["SkyBox"]->BuildObjects();
	GameObject* obj = new GameObject(1, 1);
	obj->SetMesh(0, m_umMeshes["Cube"].get());
	obj->SetMaterial(0, m_umMaterials["SkyBox"].get());
	m_umShaders["SkyBox"]->AddObject(obj);
	m_umShaders["SkyBox"]->CreateShaderVariables();

	m_umShaders["Billboard"] =
		make_unique<BillboardShader>(
			m_pd3dDevice,
			m_pd3dCommandList,
			m_pd3dGraphicsRootSignature.Get(),
			m_pd3dDescriptorHeap.Get(),
			m_nCbvSrvUavDescriptorIncrementSize);
	m_umShaders["Billboard"]->BuildObjects();

	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 20; j++) {
			GameObject* obj = new GameObject(1, 1);
			obj->SetMesh(0, m_umMeshes["Grass"].get());
			obj->SetMaterial(0, m_umMaterials["Grass"].get());

			float fx = urd(dre) * m_pTerrain->GetWidth();
			float fz = urd(dre) * m_pTerrain->GetLength();
			obj->SetPosition(Vector3::Add(XMFLOAT3(
				fx, m_pTerrain->GetHeight(fx, fz) + 0.5f, fz),
				m_pTerrain->GetPosition()));
			obj->SetScale(2.0f, 2.0f, 2.0f);

			m_umShaders["Billboard"]->AddObject(obj);
		}
	}
	m_umShaders["Billboard"]->CreateShaderVariables();
}

void MainScene::BuildTextures()
{
	// Texture( nTextures, nRootParameterIndex )
	m_umTextures["WoodCrate1"] = make_unique<Texture>(1, 3);
	m_umTextures["WoodCrate1"]->LoadTextureFromFile(
		m_pd3dDevice, m_pd3dCommandList, const_cast<wchar_t*>(L"Textures/WoodCrate01.dds"), Texture::RESOURCE_TEXTURE2D, 0);

	m_umTextures["WoodCrate2"] = make_unique<Texture>(1, 3);
	m_umTextures["WoodCrate2"]->LoadTextureFromFile(
		m_pd3dDevice, m_pd3dCommandList, const_cast<wchar_t*>(L"Textures/WoodCrate02.dds"), Texture::RESOURCE_TEXTURE2D, 0);

	m_umTextures["Base_Texture"] = make_unique<Texture>(1, 3);
	m_umTextures["Base_Texture"]->LoadTextureFromFile(
		m_pd3dDevice, m_pd3dCommandList, const_cast<wchar_t*>(L"Textures/Base_Texture.dds"), Texture::RESOURCE_TEXTURE2D, 0);

	m_umTextures["Grass"] = make_unique<Texture>(1, 3);
	m_umTextures["Grass"]->LoadTextureFromFile(
		m_pd3dDevice, m_pd3dCommandList, const_cast<wchar_t*>(L"Textures/Grass.dds"), Texture::RESOURCE_TEXTURE2D, 0);

	m_umTextures["SkyBox"] = make_unique<Texture>(1, 5);
	m_umTextures["SkyBox"]->LoadTextureFromFile(
		m_pd3dDevice, m_pd3dCommandList, const_cast<wchar_t*>(L"Textures/SkyBox.dds"), Texture::RESOURCE_TEXTURE_CUBE, 0);
}

void MainScene::BuildMaterials()
{
	m_umMaterials["WoodCrate1"] = make_unique<Material>();
	m_umMaterials["WoodCrate1"]->m_pTexture =
		m_umTextures["WoodCrate1"].get();
	m_umMaterials["WoodCrate1"]->m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_umMaterials["WoodCrate1"]->m_xmf4Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_umMaterials["WoodCrate1"]->m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_umMaterials["WoodCrate2"] = make_unique<Material>();
	m_umMaterials["WoodCrate2"]->m_pTexture =
		m_umTextures["WoodCrate2"].get();
	m_umMaterials["WoodCrate2"]->m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_umMaterials["WoodCrate2"]->m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_umMaterials["WoodCrate2"]->m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_umMaterials["Base_Texture"] = make_unique<Material>();
	m_umMaterials["Base_Texture"]->m_pTexture =
		m_umTextures["Base_Texture"].get();
	m_umMaterials["Base_Texture"]->m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_umMaterials["Grass"] = make_unique<Material>();
	m_umMaterials["Grass"]->m_pTexture =
		m_umTextures["Grass"].get();

	m_umMaterials["SkyBox"] = make_unique<Material>();
	m_umMaterials["SkyBox"]->m_pTexture =
		m_umTextures["SkyBox"].get();

	int i = 0;
	for (auto& material : m_umMaterials) {
		material.second->m_nIndex = i++;
	}
}

void MainScene::BuildLights()
{
	m_umLights["Main_Directional"] = make_unique<Light>();
	m_umLights["Main_Directional"]->m_bEnable = true;
	m_umLights["Main_Directional"]->m_nType = Light::Directional;
	m_umLights["Main_Directional"]->m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_umLights["Main_Directional"]->m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.1f, 1.0f);
	m_umLights["Main_Directional"]->m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_umLights["Main_Directional"]->m_xmf3Direction = Vector3::Normalize(XMFLOAT3(1.0f, -1.0f, 0.0f));

	m_umLights["Point1"] = make_unique<Light>();
	m_umLights["Point1"]->m_bEnable = true;
	m_umLights["Point1"]->m_nType = Light::Point;
	m_umLights["Point1"]->m_fRange = 50.0f;
	m_umLights["Point1"]->m_xmf4Ambient = XMFLOAT4(0.3f, 0.0f, 0.0f, 1.0f);
	m_umLights["Point1"]->m_xmf4Diffuse = XMFLOAT4(0.9f, 0.0f, 0.0f, 1.0f);
	m_umLights["Point1"]->m_xmf4Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f);
	m_umLights["Point1"]->m_xmf3Position =
		Vector3::Add(
			XMFLOAT3(40.0f, m_pTerrain->GetHeight(40.0f, 40.0f) + 20.0f, 40.0f),
			m_pTerrain->GetPosition());
	m_umLights["Point1"]->m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_umLights["Point2"] = make_unique<Light>();
	m_umLights["Point2"]->m_bEnable = true;
	m_umLights["Point2"]->m_nType = Light::Point;
	m_umLights["Point2"]->m_fRange = 50.0f;
	m_umLights["Point2"]->m_xmf4Ambient = XMFLOAT4(0.0f, 0.3f, 0.0f, 1.0f);
	m_umLights["Point2"]->m_xmf4Diffuse = XMFLOAT4(0.0f, 0.9f, 0.0f, 1.0f);
	m_umLights["Point2"]->m_xmf4Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f);
	m_umLights["Point2"]->m_xmf3Position =
		Vector3::Add(
			XMFLOAT3(70.0f, m_pTerrain->GetHeight(70.0f, 70.0f) + 50.0f, 70.0f),
			m_pTerrain->GetPosition());
	m_umLights["Point2"]->m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_umLights["Camera_Spot"] = make_unique<Light>();
	m_umLights["Camera_Spot"]->m_bEnable = true;
	m_umLights["Camera_Spot"]->m_nType = Light::Spot;
	m_umLights["Camera_Spot"]->m_fRange = 100.0f;
	m_umLights["Camera_Spot"]->m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_umLights["Camera_Spot"]->m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_umLights["Camera_Spot"]->m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_umLights["Camera_Spot"]->m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_umLights["Camera_Spot"]->m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_umLights["Camera_Spot"]->m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_umLights["Camera_Spot"]->m_fFalloff = 8.0f;
	m_umLights["Camera_Spot"]->m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_umLights["Camera_Spot"]->m_fTheta = (float)cos(XMConvertToRadians(20.0f));
}