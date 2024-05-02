#pragma once

#include <d3dcompiler.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
//#include <d3dx12.h>
#include "ThirdParty/Common/d3dx12.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"


#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include "../Utils/SimpleMath.h"

#ifdef _DEBUG
#pragma comment(lib, "DebugLib/DirectXTK12/DirectXTK12.lib")
#else
#pragma comment(lib, "ReleaseLib/DirectXTK12/DirectXTK12.lib")
#endif

using namespace DirectX::SimpleMath;

inline void Check(HRESULT hr)
{
	if (FAILED(hr))
	{
		MessageBox(NULL, L"��𼱰�", L"Check", MB_OK);
		assert(false);
	}
}

inline void GetErrorBlob(ID3DBlob* ErrorBlob)
{
	if (ErrorBlob != nullptr)
	{
		const char* errorMessage = (char*)ErrorBlob->GetBufferPointer();
		__debugbreak();
	}
}