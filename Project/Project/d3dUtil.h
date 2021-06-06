#pragma once

#include "pch.h"

inline wstring AnsiToWString(const string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return wstring(buffer);
}

class d3dUtil
{
public:
	static bool IsKeyDown(int vKeyCode);

	static UINT CalcConstantBufferByteSize(UINT byteSize);

	static ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		ComPtr<ID3D12Resource>& uploadBuffer);

	static ComPtr<ID3DBlob> CompileShader(
		const wstring& fileName,
		const D3D_SHADER_MACRO* defines,
		const string& entryPoint,
		const string& target);
};

class DxException
{
public:
	HRESULT ErrorCode = S_OK;
	wstring FunctionName;
	wstring FileName;
	int LineNumber = -1;

	DxException() = default;
	DxException(
		HRESULT hr,
		const wstring& functionName,
		const wstring& fileName,
		int lineNumber);

	wstring ToString() const;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
    HRESULT hr__ = (x); \
    std::wstring wfn = AnsiToWString(__FILE__); \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif