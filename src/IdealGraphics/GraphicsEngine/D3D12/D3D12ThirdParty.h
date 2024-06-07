#pragma once

#include <d3dcompiler.h>
//#include <d3dx12.h>
//#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include <d3dx12.h>
#include <dxcapi.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

#include "../Utils/SimpleMath.h"

#ifdef _DEBUG
#pragma comment(lib, "DebugLib/DirectXTK12/DirectXTK12.lib")
#else
#pragma comment(lib, "ReleaseLib/DirectXTK12/DirectXTK12.lib")
#endif

using namespace DirectX::SimpleMath;

inline void GetErrorBlob(ID3DBlob* ErrorBlob)
{
	if (ErrorBlob != nullptr)
	{
		const char* errorMessage = (char*)ErrorBlob->GetBufferPointer();
		__debugbreak();
	}
}