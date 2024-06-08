#include "GraphicsEngine/Resource/ShaderManager.h"
#include <fstream>

using namespace Ideal;

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{

}

void ShaderManager::Init()
{
	Check(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)), L"Failed To Create IDxcCompiler3");
	Check(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils)), L"Failed To Create IDxcUtils");
	Check(m_utils->CreateDefaultIncludeHandler(&m_includeHandler), L"Failed To Create IDxcIncludeHandler");
}

void ShaderManager::CompileShader(const std::wstring& FilePath, const std::wstring& ShaderModel, ComPtr<IDxcBlob>& OutBlob)
{
	// Read Shader
	std::vector<char> sourceData;
	DxcBuffer sourceBuffer;
	ReadShaderFile(FilePath, &sourceBuffer, sourceData);

	std::vector<LPCWSTR> args;
	// Shader Model
	{
		args.push_back(L"-T");
		args.push_back(ShaderModel.c_str());
	}
	// Include Directories
	{
		AddToArgumentsIncludeDirectories(args);
	}
	ComPtr<IDxcResult> result;
	m_compiler->Compile(&sourceBuffer, args.data(), args.size(), m_includeHandler.Get(), IID_PPV_ARGS(&result));

	// ERROR CHECK
	ErrorCheck(result);

	// Get Blob
	result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&OutBlob), nullptr);
}

void ShaderManager::CompileShader(const std::wstring& FilePath, const std::wstring& ShaderModel, const std::wstring& EntryPoint, ComPtr<IDxcBlob>& OutBlob)
{
	// Read Shader
	DxcBuffer sourceBuffer;
	std::vector<char> sourceData;
	ReadShaderFile(FilePath, &sourceBuffer, sourceData);

	std::vector<LPCWSTR> args;
	// Entry Point
	{
		args.push_back(L"-E");
		args.push_back(EntryPoint.c_str());
	}
	// Shader Model
	{
		args.push_back(L"-T");
		args.push_back(ShaderModel.c_str());
	}
	// Include Directories
	{
		AddToArgumentsIncludeDirectories(args);
	}
	ComPtr<IDxcResult> result;
	m_compiler->Compile(&sourceBuffer, args.data(), args.size(), m_includeHandler.Get(), IID_PPV_ARGS(&result));

	// ERROR CHECK
	ErrorCheck(result);

	// Get Blob
	result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&OutBlob), nullptr);
}

void Ideal::ShaderManager::ReadShaderFile(const std::wstring& FilePath, DxcBuffer* OutSourceBuffer, std::vector<char>& SourceData)
{
	std::ifstream shaderFile(FilePath, std::ios::binary);
	SourceData = std::vector<char>((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());

	// hlsl compile
	OutSourceBuffer->Ptr = SourceData.data();
	OutSourceBuffer->Size = SourceData.size();
	OutSourceBuffer->Encoding = DXC_CP_UTF8;
}

void ShaderManager::AddToArgumentsIncludeDirectories(std::vector<LPCWSTR>& Args)
{
	for (uint32 i = 0; i < m_includeDirectories.size(); ++i)
	{
		Args.push_back(L"-I");
		Args.push_back(m_includeDirectories[i].c_str());
	}
}

void ShaderManager::ErrorCheck(ComPtr<IDxcResult> Result)
{
	ComPtr<IDxcBlobEncoding> encoding = nullptr;
	Result->GetErrorBuffer(&encoding);
	const char* errorMessage = (char*)encoding->GetBufferPointer();
	if (errorMessage)
	{
		MyMessageBox(errorMessage);
	}
}
