#include "pch.h"
#include "d3dUtil.h"

bool d3dUtil::IsKeyDown(int vkeyCode)
{
	return (GetAsyncKeyState(vkeyCode) & 0x8000) != 0;
}

UINT d3dUtil::CalcConstantBufferByteSize(UINT byteSize)
{
	// ��� ���۴� �ּ� �ϵ���� �Ҵ� ũ�� (�Ϲ������� 256 ����Ʈ)�� ������� �Ѵ�.
	// ���� 256�� ����� �ݿø��Ѵ�.
	// 255�� ���ϰ� 256 �̸��� ��� ��Ʈ�� �����ϴ� ���� 2����Ʈ�� ����ŷ�Ͽ� �����Ѵ�.

	// �� : Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512

	return (byteSize + 255) & ~255;
}

ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 byteSize,
	ComPtr<ID3D12Resource>& uploadBuffer)
{
	ComPtr<ID3D12Resource> defaultBuffer{};

	// CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
	//
	// D3D12_HEAP_PROPERTIES heapProperties;
	// ::ZeroMemory(&heapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	// heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	// heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	// heapProperties.CreationNodeMask = 1;
	// heapProperties.VisibleNodeMask = 1;

	// CD3DX12_RESOURCE_DESC::Buffer(byteSize)
	//
	//D3D12_RESOURCE_DESC resourceDesc;
	//::ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
	//resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//resourceDesc.Alignment = 0;
	//resourceDesc.Width = byteSize;
	//resourceDesc.Height = 1;
	//resourceDesc.DepthOrArraySize = 1;
	//resourceDesc.MipLevels = 1;
	//resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	//resourceDesc.SampleDesc.Count = 1;
	//resourceDesc.SampleDesc.Quality = 0;
	//resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ����Ʈ ���۸� �����Ѵ�
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		__uuidof(ID3D12Resource),
		(void**)&defaultBuffer));

	// CPU�� �����͸� ����Ʈ ���۷� �����ϱ� ����
	// �߰� �ܰ��� ���ε� ���� �����ؾ� �Ѵ�.

	// CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
	//
	// D3D12_HEAP_PROPERTIES heapProperties;
	// ::ZeroMemory(&heapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	// heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	// heapProperties.CreationNodeMask = 1;
	// heapProperties.VisibleNodeMask = 1;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		__uuidof(ID3D12Resource),
		(void**)&uploadBuffer));

	// ����Ʈ ���ۿ� ����� �����͸� �����Ѵ�.
	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// �����͸� ����Ʈ ���ۿ� �����ϵ��� �����Ѵ�.
	// UpdateSubResources( ) �Լ��� CPU �޸𸮸� �߰� ���ε� ���� �����Ѵ�.
	// �� ���� ID3D12::CommandList::CopySubresourceRegion( )���� �߰� ���ε� �� �����Ͱ�
	// ����Ʈ ���ۿ� ����ȴ�.

	// CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
	// D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST)
	//
	// D3D12_RESOURCE_BARRIER resourceBarrier;
	// ::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	// resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// resourceBarrier.Transition.pResource = defaultBuffer.Get();
	// resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	// resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	// resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	// CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
	//	D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ)
	//
	// D3D12_RESOURCE_BARRIER resourceBarrier;
	// ::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	// resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// resourceBarrier.Transition.pResource = defaultBuffer.Get();
	// resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	// resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	// resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	return defaultBuffer;

	// ���� ���縦 �����ϴ� ����� ���� ������� �ʾ����Ƿ�
	// �� �Լ� ȣ�� �� uploadBuffer�� �������� ���ƾ� �Ѵ�.
	// ȣ���ڴ� ���簡 ����� ���� �˰� ���� uploadBuffer�� ������ �� �ִ�.
}

ComPtr<ID3DBlob> d3dUtil::CompileShader(
	const wstring& fileName,
	const D3D_SHADER_MACRO* defines,
	const string& entryPoint,
	const string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(fileName.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	return byteCode;
}

DxException::DxException(
	HRESULT hr,
	const wstring& functionName,
	const wstring& fileName,
	int lineNumber)
	: ErrorCode{ hr },
	FunctionName{ functionName },
	FileName{ fileName },
	LineNumber{ lineNumber }
{}

wstring DxException::ToString() const
{
	_com_error err(ErrorCode);
	wstring msg = err.ErrorMessage();

	return FunctionName + L" failed in " + FileName + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}