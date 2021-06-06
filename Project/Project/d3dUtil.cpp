#include "pch.h"
#include "d3dUtil.h"

bool d3dUtil::IsKeyDown(int vkeyCode)
{
	return (GetAsyncKeyState(vkeyCode) & 0x8000) != 0;
}

UINT d3dUtil::CalcConstantBufferByteSize(UINT byteSize)
{
	// 상수 버퍼는 최소 하드웨어 할당 크기 (일반적으로 256 바이트)의 배수여야 한다.
	// 따라서 256의 배수로 반올림한다.
	// 255를 더하고 256 미만의 모든 비트를 저장하는 하위 2바이트를 마스킹하여 수행한다.

	// 예 : Suppose byteSize = 300.
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

	// 디폴트 버퍼를 생성한다
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		__uuidof(ID3D12Resource),
		(void**)&defaultBuffer));

	// CPU의 데이터를 디폴트 버퍼로 복사하기 위해
	// 중간 단계의 업로드 힙을 생성해야 한다.

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

	// 디폴트 버퍼에 복사될 데이터를 서술한다.
	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// 데이터를 디폴트 버퍼에 복사하도록 예약한다.
	// UpdateSubResources( ) 함수는 CPU 메모리를 중간 업로드 힙에 복사한다.
	// 그 다음 ID3D12::CommandList::CopySubresourceRegion( )으로 중간 업로드 힙 데이터가
	// 디폴트 버퍼에 복사된다.

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

	// 실제 복사를 수행하는 명령이 아직 실행되지 않았으므로
	// 위 함수 호출 후 uploadBuffer가 해제되지 말아야 한다.
	// 호출자는 복사가 실행된 것을 알고 나면 uploadBuffer를 해제할 수 있다.
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