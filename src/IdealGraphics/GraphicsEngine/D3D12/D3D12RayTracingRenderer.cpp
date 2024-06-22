#include "D3D12RayTracingRenderer.h"

#include <DirectXColors.h>
#include "Misc/Utils/PIX.h"
#include "Misc/Assimp/AssimpConverter.h"

#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/Raytracing/D3D12RaytracingResources.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12RootSignature.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12Viewport.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/D3D12/D3D12SRV.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"
#include "GraphicsEngine/D3D12/Raytracing/RaytracingManager.h"

#include "GraphicsEngine/Resource/ShaderManager.h"
#include "GraphicsEngine/Resource/IdealCamera.h"
#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealAnimation.h"
#include "GraphicsEngine/Resource/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealScreenQuad.h"
#include "GraphicsEngine/Resource/IdealRenderScene.h"
#include "GraphicsEngine/Resource/IdealRaytracingRenderScene.h"

#include "GraphicsEngine/Resource/Light/IdealDirectionalLight.h"
#include "GraphicsEngine/Resource/Light/IdealSpotLight.h"
#include "GraphicsEngine/Resource/Light/IdealPointLight.h"


#include "GraphicsEngine/D3D12/TestShader.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)


#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <istream>
#include <fstream>
#include <vector>
// Pretty-print a state object tree.
inline void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc)
{
	std::wstringstream wstr;
	wstr << L"\n";
	wstr << L"--------------------------------------------------------------------\n";
	wstr << L"| D3D12 State Object 0x" << static_cast<const void*>(desc) << L": ";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_COLLECTION) wstr << L"Collection\n";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) wstr << L"Raytracing Pipeline\n";

	auto ExportTree = [](UINT depth, UINT numExports, const D3D12_EXPORT_DESC* exports)
		{
			std::wostringstream woss;
			for (UINT i = 0; i < numExports; i++)
			{
				woss << L"|";
				if (depth > 0)
				{
					for (UINT j = 0; j < 2 * depth - 1; j++) woss << L" ";
				}
				woss << L" [" << i << L"]: ";
				if (exports[i].ExportToRename) woss << exports[i].ExportToRename << L" --> ";
				woss << exports[i].Name << L"\n";
			}
			return woss.str();
		};

	for (UINT i = 0; i < desc->NumSubobjects; i++)
	{
		wstr << L"| [" << i << L"]: ";
		switch (desc->pSubobjects[i].Type)
		{
			case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
				wstr << L"Global Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
				wstr << L"Local Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
				wstr << L"Node Mask: 0x" << std::hex << std::setfill(L'0') << std::setw(8) << *static_cast<const UINT*>(desc->pSubobjects[i].pDesc) << std::setw(0) << std::dec << L"\n";
				break;
			case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
			{
				wstr << L"DXIL Library 0x";
				auto lib = static_cast<const D3D12_DXIL_LIBRARY_DESC*>(desc->pSubobjects[i].pDesc);
				wstr << lib->DXILLibrary.pShaderBytecode << L", " << lib->DXILLibrary.BytecodeLength << L" bytes\n";
				wstr << ExportTree(1, lib->NumExports, lib->pExports);
				break;
			}
			case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
			{
				wstr << L"Existing Library 0x";
				auto collection = static_cast<const D3D12_EXISTING_COLLECTION_DESC*>(desc->pSubobjects[i].pDesc);
				wstr << collection->pExistingCollection << L"\n";
				wstr << ExportTree(1, collection->NumExports, collection->pExports);
				break;
			}
			case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
			{
				wstr << L"Subobject to Exports Association (Subobject [";
				auto association = static_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
				UINT index = static_cast<UINT>(association->pSubobjectToAssociate - desc->pSubobjects);
				wstr << index << L"])\n";
				for (UINT j = 0; j < association->NumExports; j++)
				{
					wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
				}
				break;
			}
			case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
			{
				wstr << L"DXIL Subobjects to Exports Association (";
				auto association = static_cast<const D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
				wstr << association->SubobjectToAssociate << L")\n";
				for (UINT j = 0; j < association->NumExports; j++)
				{
					wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
				}
				break;
			}
			case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
			{
				wstr << L"Raytracing Shader Config\n";
				auto config = static_cast<const D3D12_RAYTRACING_SHADER_CONFIG*>(desc->pSubobjects[i].pDesc);
				wstr << L"|  [0]: Max Payload Size: " << config->MaxPayloadSizeInBytes << L" bytes\n";
				wstr << L"|  [1]: Max Attribute Size: " << config->MaxAttributeSizeInBytes << L" bytes\n";
				break;
			}
			case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
			{
				wstr << L"Raytracing Pipeline Config\n";
				auto config = static_cast<const D3D12_RAYTRACING_PIPELINE_CONFIG*>(desc->pSubobjects[i].pDesc);
				wstr << L"|  [0]: Max Recursion Depth: " << config->MaxTraceRecursionDepth << L"\n";
				break;
			}
			case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
			{
				wstr << L"Hit Group (";
				auto hitGroup = static_cast<const D3D12_HIT_GROUP_DESC*>(desc->pSubobjects[i].pDesc);
				wstr << (hitGroup->HitGroupExport ? hitGroup->HitGroupExport : L"[none]") << L")\n";
				wstr << L"|  [0]: Any Hit Import: " << (hitGroup->AnyHitShaderImport ? hitGroup->AnyHitShaderImport : L"[none]") << L"\n";
				wstr << L"|  [1]: Closest Hit Import: " << (hitGroup->ClosestHitShaderImport ? hitGroup->ClosestHitShaderImport : L"[none]") << L"\n";
				wstr << L"|  [2]: Intersection Import: " << (hitGroup->IntersectionShaderImport ? hitGroup->IntersectionShaderImport : L"[none]") << L"\n";
				break;
			}
		}
		wstr << L"|--------------------------------------------------------------------\n";
	}
	wstr << L"\n";
	OutputDebugStringW(wstr.str().c_str());
}

inline void AllocateUploadBuffer(ID3D12Device* pDevice, void* pData, UINT64 datasize, ID3D12Resource** ppResource, const wchar_t* resourceName = nullptr)
{
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(datasize);
	Check(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName)
	{
		(*ppResource)->SetName(resourceName);
	}
	void* pMappedData;
	(*ppResource)->Map(0, nullptr, &pMappedData);
	memcpy(pMappedData, pData, datasize);
	(*ppResource)->Unmap(0, nullptr);
}

inline void AllocateUAVBuffer(ID3D12Device* pDevice, UINT64 bufferSize, ID3D12Resource** ppResource, D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, const wchar_t* resourceName = nullptr)
{
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	Check(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialResourceState,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName)
	{
		(*ppResource)->SetName(resourceName);
	}
}

Ideal::D3D12RayTracingRenderer::D3D12RayTracingRenderer(HWND hwnd, uint32 Width, uint32 Height, bool EditorMode)
	: m_hwnd(hwnd),
	m_width(Width),
	m_height(Height),
	m_frameIndex(0),
	m_fenceEvent(NULL),
	m_fenceValue(0)
{
	m_aspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
}

Ideal::D3D12RayTracingRenderer::~D3D12RayTracingRenderer()
{
	Fence();
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		WaitForFenceValue(m_lastFenceValues[i]);
	}
	//m_vertexBufferShaderTableHandle.Free();
	//m_indexBufferShaderTableHandle.Free();
	//m_textureShaderTableHandle.Free();

	m_raytacingOutputResourceUAVGpuDescriptorHandle.Free();

	m_indexBufferShaderTableHandle.Free();
	m_vertexBufferShaderTableHandle.Free();
	m_textureShaderTableHandle.Free();

	m_texture = nullptr;
	m_texture2 = nullptr;
	m_vertexBufferSRV = nullptr;
	m_indexBufferSRV = nullptr;
	m_resourceManager = nullptr;
}

void Ideal::D3D12RayTracingRenderer::Init()
{
	uint32 dxgiFactoryFlags = 0;

#ifdef _DEBUG
	// Check to see if a copy of WinPixGpuCapturer.dll has already been injected into the application.
	// This may happen if the application is launched through the PIX UI. 
	if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
	{
		LoadLibrary(GetLatestWinPixGpuCapturerPath().c_str());
	}
#endif

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ComPtr<ID3D12Debug1> debugController1;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			/*if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1))))
			{
				debugController1->SetEnableGPUBasedValidation(true);
			}*/
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	//---------------------Viewport Init-------------------------//
	m_viewport = std::make_shared<Ideal::D3D12Viewport>(m_hwnd, m_width, m_height);
	m_viewport->Init();

	//---------------------Create Device-------------------------//
	ComPtr<IDXGIFactory6> factory;
	Check(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf())), L"Failed To Create DXGIFactory1");

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1
	};
	DWORD featureLevelNum = _countof(featureLevels);
	DXGI_ADAPTER_DESC1 adapterDesc = {};
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	for (uint32 featureLevelIndex = 0; featureLevelIndex < featureLevelNum; ++featureLevelIndex)
	{
		uint32 adapterIndex = 0;
		while (DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter))
		{
			adapter->GetDesc1(&adapterDesc);

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), featureLevels[featureLevelIndex], IID_PPV_ARGS(m_device.GetAddressOf()))))
			{
				DXGI_ADAPTER_DESC1 desc = {};
				adapter->GetDesc1(&desc);

				goto finishAdapter;
				break;
			}
			adapter = nullptr;
			adapterIndex++;
		}
	}
finishAdapter:

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 caps = {};
	Check(m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &caps, sizeof(caps)), L"Device dose not support ray tracing!");
	if (caps.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
	{
		MyMessageBoxW(L"Device dose not support ray tracing!");
	}

	//---------------Command Queue-----------------//
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	Check(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

	//---------------Create rendering resource---------------//
	CreateCommandlists();
	CreateSwapChains(factory);
	CreateRTV();
	CreateDSVHeap();
	CreateDSV(m_width, m_height);
	CreateFence();
	CreatePools();

	m_resourceManager = std::make_shared<Ideal::ResourceManager>();
	m_resourceManager->Init(m_device);
	m_resourceManager->SetAssetPath(m_assetPath);
	m_resourceManager->SetModelPath(m_modelPath);
	m_resourceManager->SetTexturePath(m_texturePath);

	m_shaderManager = std::make_shared<Ideal::ShaderManager>();
	m_shaderManager->Init();
	m_shaderManager->AddIncludeDirectories(L"../Shaders/Raytracing/");

	CreateDescriptorHeap();
	// --- Test --- //
	//`Compiler();
	m_rayGenCB.viewport = { -1.f,-1.f,1.f,1.f };
	UpdateForSizeChange(m_width, m_height);

	// shader test
	//InitShader();
	//CompileShader2(L"../Shaders/Raytracing/Raytracing.hlsl", m_raygenShaderName, m_testBlob);
	//m_shaderManager->CompileShader(L"../Shaders/Raytracing/Raytracing.hlsl", L"lib_6_3", m_testBlob);
	m_shaderManager->CompileShaderAndSave(
		L"../Shaders/Raytracing/Raytracing.hlsl",
		L"../Shaders/Raytracing/",
		L"SimpleRaytracingShader3",
		L"lib_6_3",
		m_testBlob
	);
	m_shaderManager->LoadShaderFile(L"../Shaders/Raytracing/SimpleRaytracingShader3.shader", m_myShader);
	//
	m_cubeCB.albedo = Vector4(1.f, 1.f, 1.f, 1.f);
	//m_sceneCB.CameraPos.x = 0;
	//m_sceneCB.CameraPos.y = 0;
	//m_sceneCB.CameraPos.z = 0;

	m_sceneCB.CameraPos = Vector4(0.f);

	m_sceneCB.lightPos = Vector4(3.f, 1.8f, -3.f, 0.f);
	//m_sceneCB.lightPos = Vector4(0.5f, 0.5f, -5.f, 0.f);
	m_sceneCB.lightAmbient = Vector4(0.5f, 0.5f, 0.5f, 1.f);
	//m_sceneCB.lightDiffuse = Vector4(0.5f, 0.f, 0.f, 1.f);
	m_sceneCB.lightDiffuse = Vector4(0.5f, 0.f, 0.f, 1.f);
	// load image
	m_resourceManager->CreateTexture(m_texture, L"../Resources/Textures/Any/anyImage.jpg");
	m_resourceManager->CreateTexture(m_texture2, L"../Resources/Textures/Any/anyImage2.png");

	// create resource
	CreateDeviceDependentResources();
	RaytracingManagerInit();
}

void Ideal::D3D12RayTracingRenderer::Tick()
{
	// 테스트 용으로 쓰는 곳
	__debugbreak();
	return;
}

void Ideal::D3D12RayTracingRenderer::Render()
{
	m_mainCamera->UpdateMatrix2();

	m_sceneCB.CameraPos = m_mainCamera->GetPosition();
	m_sceneCB.ProjToWorld = m_mainCamera->GetViewProj().Invert().Transpose();


	ResetCommandList();
	BeginRender();
	//test
	if (GetAsyncKeyState('B') & 0x8000)
	{
		static float c = 0;
		if (c >= 1.f)
		{
			c = 0.f;
		}
		c += 0.001f;
		m_cubeCB.albedo = Color(c, c, c, c);
	}
	// 메인 박스 레이트레이싱
	//UpdateAccelerationStructure();
	//DoRaytracing();

	// Raytracing Manager 레이트레이싱
	//RaytracingManagerUpdate();
	//DoRaytracing4();

	// Raytracing Manager2 레이트레이싱
	//RaytracingManagerUpdate();
	//m_raytracingManager->DispatchRays(
	//	m_device,
	//	m_commandLists[m_currentContextIndex],
	//	m_descriptorHeaps[m_currentContextIndex],
	//	m_raytacingOutputResourceUAVGpuDescriptorHandle,
	//	m_cbAllocator[m_currentContextIndex], m_width, m_height, m_sceneCB);

	// 3
	RaytracingManagerUpdate();
	DoRaytracing5();


	//UpdateAccelerationStructure();
	//BuildShaderTables2();
	//DoRaytracing2();

	//TestDrawRenderScene();
	//BuildShaderTables2();
	//DoRaytracing3();

	//BuildShaderTables2();
	//RaytracingManagerUpdate();
	//DoRaytracing4();

	CopyRaytracingOutputToBackBuffer();
	EndRender();
	Present();
	return;
}

void Ideal::D3D12RayTracingRenderer::Resize(UINT Width, UINT Height)
{
}

std::shared_ptr<Ideal::ICamera> Ideal::D3D12RayTracingRenderer::CreateCamera()
{
	std::shared_ptr<Ideal::IdealCamera> newCamera = std::make_shared<Ideal::IdealCamera>();
	newCamera->SetLens(0.25f * 3.141592f, m_aspectRatio, 1.f, 3000.f);
	return newCamera;
}

void Ideal::D3D12RayTracingRenderer::SetMainCamera(std::shared_ptr<ICamera> Camera)
{
	m_mainCamera = std::static_pointer_cast<Ideal::IdealCamera>(Camera);
}

std::shared_ptr<Ideal::IMeshObject> Ideal::D3D12RayTracingRenderer::CreateStaticMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealStaticMeshObject> newStaticMesh = std::make_shared<Ideal::IdealStaticMeshObject>();
	m_resourceManager->CreateStaticMeshObject(newStaticMesh, FileName);

	//newStaticMesh->Init(m_device);

	// temp
	m_staticMeshObject = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(newStaticMesh);
	RaytracingManagerAddObject(m_staticMeshObject);
	return newStaticMesh;
	//return nullptr;
}

std::shared_ptr<Ideal::ISkinnedMeshObject> Ideal::D3D12RayTracingRenderer::CreateSkinnedMeshObject(const std::wstring& FileName)
{
	// 인터페이스로 따로 뽑아야 할 듯
	return nullptr;
}

std::shared_ptr<Ideal::IAnimation> Ideal::D3D12RayTracingRenderer::CreateAnimation(const std::wstring& FileName)
{
	// 인터페이스로 따로 뽑아야 할 듯
	return nullptr;
}

std::shared_ptr<Ideal::IRenderScene> Ideal::D3D12RayTracingRenderer::CreateRenderScene()
{
	ResetCommandList();

	std::shared_ptr<Ideal::IdealRaytracingRenderScene> ret = std::make_shared<Ideal::IdealRaytracingRenderScene>();
	ret->Init(m_device, m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex]);

	m_commandLists[m_currentContextIndex]->Close();
	ID3D12CommandList* ppCommandLists[] = { m_commandLists[m_currentContextIndex].Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Fence();
	WaitForFenceValue(m_lastFenceValues[m_currentContextIndex]);
	return ret;
}

void Ideal::D3D12RayTracingRenderer::SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene)
{
	// 인터페이스로 따로 뽑아야 할 듯
	std::shared_ptr<Ideal::IdealRaytracingRenderScene> renderScene = std::static_pointer_cast<Ideal::IdealRaytracingRenderScene>(RenderScene);
	m_renderScene = renderScene;
}

std::shared_ptr<Ideal::IDirectionalLight> Ideal::D3D12RayTracingRenderer::CreateDirectionalLight()
{
	// 인터페이스로 따로 뽑아야 할 듯
	return nullptr;
}

std::shared_ptr<Ideal::ISpotLight> Ideal::D3D12RayTracingRenderer::CreateSpotLight()
{
	// 인터페이스로 따로 뽑아야 할 듯
	return nullptr;
}

std::shared_ptr<Ideal::IPointLight> Ideal::D3D12RayTracingRenderer::CreatePointLight()
{
	// 인터페이스로 따로 뽑아야 할 듯
	return nullptr;
}

void Ideal::D3D12RayTracingRenderer::SetAssetPath(const std::wstring& AssetPath)
{
	m_assetPath = AssetPath;
}

void Ideal::D3D12RayTracingRenderer::SetModelPath(const std::wstring& ModelPath)
{
	m_modelPath = ModelPath;
}

void Ideal::D3D12RayTracingRenderer::SetTexturePath(const std::wstring& TexturePath)
{
	m_texturePath = TexturePath;
}

void Ideal::D3D12RayTracingRenderer::ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData /*= false*/, bool NeedVertexInfo /*= false*/)
{
}

void Ideal::D3D12RayTracingRenderer::ConvertAnimationAssetToMyFormat(std::wstring FileName)
{
}

bool Ideal::D3D12RayTracingRenderer::SetImGuiWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void Ideal::D3D12RayTracingRenderer::ClearImGui()
{
}

void Ideal::D3D12RayTracingRenderer::CreateSwapChains(ComPtr<IDXGIFactory6> Factory)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;


	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
	fullScreenDesc.RefreshRate.Numerator = 60;
	fullScreenDesc.RefreshRate.Denominator = 1;
	fullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	fullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	fullScreenDesc.Windowed = true;

	m_swapChainFlags = swapChainDesc.Flags;

	ComPtr<IDXGISwapChain1> swapChain;
	Check(Factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		m_hwnd,
		&swapChainDesc,
		&fullScreenDesc,
		nullptr,
		swapChain.GetAddressOf()
	));

	Check(Factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER), L"Failed to make window association");
	Check(swapChain.As(&m_swapChain), L"Failed to change swapchain version");
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

}

void Ideal::D3D12RayTracingRenderer::CreateRTV()
{
	m_rtvHeap = std::make_shared<Ideal::D3D12DescriptorHeap>();
	m_rtvHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, SWAP_CHAIN_FRAME_COUNT);
	//m_rtvHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, SWAP_CHAIN_FRAME_COUNT);

	float c[4] = { 0.f,0.f,0.f,1.f };
	D3D12_CLEAR_VALUE clearValue = {};
	//clearValue.Format = swapChainDesc.Format;
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clearValue.Color[0] = c[0];
	clearValue.Color[1] = c[1];
	clearValue.Color[2] = c[2];
	clearValue.Color[3] = c[3];

	{
		// Create RenderTarget Texture
		for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
		{
			m_renderTargets[i] = std::make_shared<Ideal::D3D12Texture>();
		}

		for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
		{
			auto handle = m_rtvHeap->Allocate();
			ComPtr<ID3D12Resource> resource;
			Check(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&resource)));
			m_device->CreateRenderTargetView(resource.Get(), nullptr, handle.GetCpuHandle());
			m_renderTargets[i]->Create(resource);
			m_renderTargets[i]->EmplaceRTV(handle);
		}
	}
}

void Ideal::D3D12RayTracingRenderer::CreateDSVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	Check(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

}

void Ideal::D3D12RayTracingRenderer::CreateDSV(uint32 Width, uint32 Height)
{
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		Width,
		Height,
		1,
		0,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	Check(m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(m_depthStencil.GetAddressOf())
	));

	// create dsv
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_device->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

}

void Ideal::D3D12RayTracingRenderer::CreateFence()
{
	Check(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())), L"Failed to create fence!");
	m_fenceValue = 0;
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void Ideal::D3D12RayTracingRenderer::CreateCommandlists()
{
	// CreateCommand List
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		Check(
			m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocators[i].GetAddressOf())),
			L"Failed to create command allocator!"
		);

		Check(
			m_device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_commandAllocators[i].Get(),
				nullptr,
				IID_PPV_ARGS(m_commandLists[i].GetAddressOf())),
			L"Failed to create command list"
		);
		Check(m_commandLists[i]->Close(), L"Failed to close commandlist");
	}

}

void Ideal::D3D12RayTracingRenderer::CreatePools()
{
	// descriptor heap, constant buffer pool Allocator
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		// descriptor heap
		m_descriptorHeaps[i] = std::make_shared<Ideal::D3D12DescriptorHeap>();
		m_descriptorHeaps[i]->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_DESCRIPTOR_COUNT);

		// Dynamic CB Pool Allocator
		m_cbAllocator[i] = std::make_shared<Ideal::D3D12DynamicConstantBufferAllocator>();
		m_cbAllocator[i]->Init(MAX_DRAW_COUNT_PER_FRAME);

		// Upload Pool
		m_BLASInstancePool[i] = std::make_shared<Ideal::D3D12UploadBufferPool>();
		m_BLASInstancePool[i]->Init(m_device.Get(), sizeof(Ideal::DXRInstanceDesc), MAX_DRAW_COUNT_PER_FRAME);

		// shader table Descriptor Heap
		//m_shaderTableDescriptorHeaps[i] = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
		//m_shaderTableDescriptorHeaps[i]->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_DESCRIPTOR_COUNT);
	}
}

void Ideal::D3D12RayTracingRenderer::CreateDefaultCamera()
{
	m_mainCamera = std::make_shared<Ideal::IdealCamera>();
	std::shared_ptr<Ideal::IdealCamera> camera = std::static_pointer_cast<Ideal::IdealCamera>(m_mainCamera);
	camera->SetLens(0.25f * 3.141592f, m_aspectRatio, 1.f, 3000.f);
}

void Ideal::D3D12RayTracingRenderer::ResetCommandList()
{
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandAllocators[m_currentContextIndex];
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandLists[m_currentContextIndex];

	Check(commandAllocator->Reset(), L"Failed to reset commandAllocator!");
	Check(commandList->Reset(commandAllocator.Get(), nullptr), L"Failed to reset commandList");
}

void Ideal::D3D12RayTracingRenderer::BeginRender()
{
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandAllocators[m_currentContextIndex];
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandLists[m_currentContextIndex];

	CD3DX12_RESOURCE_BARRIER backBufferBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex]->GetResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &backBufferBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	auto rtvHandle = m_renderTargets[m_frameIndex]->GetRTV();


	commandList->ClearRenderTargetView(rtvHandle.GetCpuHandle(), DirectX::Colors::Black, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->RSSetViewports(1, &m_viewport->GetViewport());
	commandList->RSSetScissorRects(1, &m_viewport->GetScissorRect());
	commandList->OMSetRenderTargets(1, &rtvHandle.GetCpuHandle(), FALSE, &dsvHandle);
}

void Ideal::D3D12RayTracingRenderer::EndRender()
{
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandLists[m_currentContextIndex];

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex]->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	commandList->ResourceBarrier(1, &barrier);
	Check(commandList->Close());
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void Ideal::D3D12RayTracingRenderer::Present()
{
	Fence();

	HRESULT hr;

	uint32 SyncInterval = 0;
	uint32 PresentFlags = 0;
	PresentFlags = DXGI_PRESENT_ALLOW_TEARING;

	hr = m_swapChain->Present(0, PresentFlags);
	Check(hr);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	uint32 nextContextIndex = (m_currentContextIndex + 1) % MAX_PENDING_FRAME_COUNT;
	WaitForFenceValue(m_lastFenceValues[nextContextIndex]);

	m_descriptorHeaps[nextContextIndex]->Reset();
	m_cbAllocator[nextContextIndex]->Reset();
	m_BLASInstancePool[nextContextIndex]->Reset();

	m_currentContextIndex = nextContextIndex;
}

uint64 Ideal::D3D12RayTracingRenderer::Fence()
{
	m_fenceValue++;
	m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
	m_lastFenceValues[m_currentContextIndex] = m_fenceValue;

	return m_fenceValue;
}

void Ideal::D3D12RayTracingRenderer::WaitForFenceValue(uint64 ExpectedFenceValue)
{
	if (m_fence->GetCompletedValue() < ExpectedFenceValue)
	{
		m_fence->SetEventOnCompletion(ExpectedFenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void Ideal::D3D12RayTracingRenderer::CreateDeviceDependentResources()
{
	CreateRootSignatures();
	CreateRaytracingPipelineStateObject();
	BuildGeometry();
	BuildAccelerationStructures();
	//BuildAccelerationStructures2();
	BuildShaderTables();
	CreateRayTracingOutputResources();
}

void Ideal::D3D12RayTracingRenderer::CompileShader2(const std::wstring& FilePath, const std::wstring& EntryPoint, ComPtr<IDxcBlob>& OutBlob)
{
	ComPtr<IDxcCompiler3> compiler;
	ComPtr<IDxcUtils> utils;
	ComPtr<IDxcIncludeHandler> includeHandler;
	// Create compiler
	Check(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)), L"Failed To Create DXC Compiler Instance");
	Check(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)), L"Failed To Create DXC Compiler Instance");
	std::ifstream shaderFile(FilePath, std::ios::binary);
	// temp
	std::vector<char> shaderSource((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());

	// hlsl compile
	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = shaderSource.data();
	sourceBuffer.Size = shaderSource.size();
	sourceBuffer.Encoding = DXC_CP_UTF8;

	// header
	ComPtr<IDxcBlob> includeSource = nullptr;
	utils->CreateDefaultIncludeHandler(&includeHandler);
	//includeHandler->LoadSource(L"../Shaders/Raytracing/RaytracingHlslCompat.h", &includeSource);
	//includeHandler->LoadSource(L"RaytracingHlslCompat.h", &includeSource);

	LPCWSTR args[] = {
		//L"-E", EntryPoint.c_str(),	// raytracing 쉐이더는 엔트리포인트가 필요 없음
		L"-T", L"lib_6_3",
		L"-I", L"../Shaders/Raytracing/"
	};
	ComPtr<IDxcResult> result;
	//compiler->Compile(&sourceBuffer, nullptr, 0, includeHandler.Get(), IID_PPV_ARGS(&result));
	compiler->Compile(&sourceBuffer, args, _countof(args), includeHandler.Get(), IID_PPV_ARGS(&result));


	ComPtr<IDxcBlobEncoding> encoding = nullptr;
	result->GetErrorBuffer(&encoding);
	if (encoding)
	{
		auto a = (char*)encoding->GetBufferPointer();
		int b = 3;
	}

	result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&OutBlob), nullptr);
}

void Ideal::D3D12RayTracingRenderer::CreateRayTracingInterfaces()
{
	//Check(m_device->QueryInterface(IID_PPV_ARGS(&m_device)), L"F");
	//for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	//{
	//	Check(m_commandLists[i]->QueryInterface(IID_PPV_ARGS(m_commandLists[i].GetAddressOf())), L"F");
	//}
}

void Ideal::D3D12RayTracingRenderer::CreateRootSignatures()
{
	// 수정 지금 뭐 바뀌는 슬롯은 없으니 그냥 하나의 range로 모두 처리해버리겠다
	{
		CD3DX12_DESCRIPTOR_RANGE rangeRenderTarget[2];
		rangeRenderTarget[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		CD3DX12_DESCRIPTOR_RANGE rangeGlobalCB[1];
		rangeGlobalCB[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		CD3DX12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].InitAsDescriptorTable(1, rangeRenderTarget);
		rootParameters[1].InitAsShaderResourceView(0);
		rootParameters[2].InitAsDescriptorTable(1, rangeGlobalCB);

		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(_countof(rootParameters), rootParameters);

		// TEMP : 임시 Sampler
		CD3DX12_STATIC_SAMPLER_DESC sampler(
			0,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP
		);
		globalRootSignatureDesc.NumStaticSamplers = 1;
		globalRootSignatureDesc.pStaticSamplers = &sampler;

		SerializeAndCreateRayTracingRootSignature(globalRootSignatureDesc, &m_raytracingGlobalRootSignature);
	}

	// Local Root Signature
	// 이 루트 시그니쳐는 쉐이더 테이블에서 나온 유니크 인자들을 쉐이더가 가질 수 있게 한다.
	{
		//---space1---//
		CD3DX12_DESCRIPTOR_RANGE ranges[3];
		//ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 1);	// cube b0
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1);	// t0 : indices 
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 1);	// t1 : vertices
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 1); // t2 : diffuse

		CD3DX12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);	// indices
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);	// vertices
		rootParameters[2].InitAsDescriptorTable(1, &ranges[2]);	// diffuse

		CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		SerializeAndCreateRayTracingRootSignature(localRootSignatureDesc, &m_raytracingLocalRootSignature);
	}
}

void Ideal::D3D12RayTracingRenderer::SerializeAndCreateRayTracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootsig)
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;

	// 루트 시그니쳐 직렬화
	Check(
		D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error),
		error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr
	);
	/*if (error)
	{
		auto a = static_cast<wchar_t*>(error->GetBufferPointer());
		int b = 3;
	}*/
	// 루트 시그니쳐 생성
	Check(
		m_device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootsig))),
		L"Failed to Create RootSignature!"
	);
}

void Ideal::D3D12RayTracingRenderer::CreateRaytracingPipelineStateObject()
{
	// 7개의 subobject를 만들어 RTPSO와 결합한다. (ray tracing pipe line state sobject)
	// subobject들은 default 연결 또는 명시적 연결을 통해 DXIL 익스포트(즉, 셰이더)와 연결해야 한다.
	// default 연결은 익스포트된 모든 셰이더 엔트리 포인트에 동일한 유형의 서브오브젝트가 연결되지 않은 모든 셰이더 엔트리포인트에 적용이 된다.
	// 이 샘플은 데모용으로 명시적으로 지정된 로컬 루트 서명 서브 오브젝트를 제외한 기본 셰이더 연결을 활용
	// 1 - DXIL library
	// 1 - Triangle hit group
	// 1 - shader config
	// 2 - local root signature and association
	// 1 - Global root signature
	// 1 - pipeline config
	CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

	//-------------------DXIL library-------------------//
	// 이것은 스테이트 오브젝트를 위해 쉐이더들과 쉐이더들의 엔트리 포인트들을 포함한다.
	// 셰이더는 서브오브젝트로 간주되지 않으므로 DXIL 라이브러리 서브오브젝트를 통해 전달해야 한다.
	CD3DX12_DXIL_LIBRARY_SUBOBJECT* lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	//D3D12_SHADER_BYTECODE libdxil2 = CD3DX12_SHADER_BYTECODE(m_testBlob->GetBufferPointer(), m_testBlob->GetBufferSize());
	D3D12_SHADER_BYTECODE libdxil2 = CD3DX12_SHADER_BYTECODE(m_myShader->GetBufferPointer(), m_myShader->GetSize());
	//D3D12_SHADER_BYTECODE libdxil2 = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracing2, ARRAYSIZE(g_pRaytracing2));
	lib->SetDXILLibrary(&libdxil2);
	lib->DefineExport(m_raygenShaderName);
	lib->DefineExport(m_closestHitShaderName);
	lib->DefineExport(m_missShaderName);


	//-------------------Triangle hit group-------------------//
	// 히트 그룹은 광선이 지오메트리의 삼각형 / AABB와 교차할 때 실행할 가장 가까운 hit, any hit 및 intersection 쉐이더를 지정
	// 이 샘플에서는 closest shader가 있는 삼각형 지오메트리만 사용하므로 다른 쉐이더는 설정하지 않았다.
	CD3DX12_HIT_GROUP_SUBOBJECT* hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetClosestHitShaderImport(m_closestHitShaderName);
	hitGroup->SetHitGroupExport(m_hitGroupName);
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// Shader Config
	// ray payload 와 attribute structure의 최대 크기를 바이트 단위로 정의한다.
	CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT* shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	uint32 payloadSize = sizeof(Color); // float4 color
	uint32 attributeSize = 2 * sizeof(float); //float 2 barycentrics (질량 중심 좌표?)
	shaderConfig->Config(payloadSize, attributeSize);

	// Local root signature and shader association
	CreateLocalRootSignatureSubobjects(&raytracingPipeline);

	// Global root signature
	CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT* globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

	// Pipeline config
	// TraceRay() 재귀 depths의 최대를 정의한다
	CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT* pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	// 퍼포먼스 팁 : 드라이버가 낮은 재귀 깊이에 대한 최적화 전략을 적용할 수 있으므로 최대 재귀 깊이를 필요한 만큼 낮게 설정해라
	uint32 maxRecursionDepth = 1; // 재귀 한번만
	pipelineConfig->Config(maxRecursionDepth);

	PrintStateObjectDesc(raytracingPipeline);
	//raytracingPipeline;
	Check(
		m_device->CreateStateObject(
			raytracingPipeline,
			IID_PPV_ARGS(m_dxrStateObject.GetAddressOf())),
		L"Failed to create DirectX Raytracing state object"
	);
}

void Ideal::D3D12RayTracingRenderer::CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
{
	CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT* localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
	localRootSignature->SetRootSignature(m_raytracingLocalRootSignature.Get());

	// Shader association  연관성?
	CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT* rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
	rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
	rootSignatureAssociation->AddExport(m_hitGroupName);
}

void Ideal::D3D12RayTracingRenderer::BuildGeometry()
{
	std::vector<uint32> indices = {
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22 };

	std::vector<PositionNormalUVVertex> vertices;
	{
		using namespace DirectX;
		vertices =
		{
			//// Top face
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

			// Bottom face
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

			// Left face
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

			// Right face
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

			// Front face
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

			// Back face
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }

			/*{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },*/
		};
	}


	m_vertexBuffer = std::make_shared<Ideal::D3D12VertexBuffer>();
	m_indexBuffer = std::make_shared<Ideal::D3D12IndexBuffer>();

	m_resourceManager->CreateVertexBuffer(m_vertexBuffer, vertices);
	m_resourceManager->CreateIndexBuffer(m_indexBuffer, indices);
	//m_resourceManager->CreateIndexBufferUint16(m_indexBuffer, indices);

	//m_indexBufferSRV = m_resourceManager->CreateSRV(m_indexBuffer, m_indexBuffer->GetElementCount(), 0);
	m_indexBufferSRV = m_resourceManager->CreateSRV(m_indexBuffer, m_indexBuffer->GetElementCount(), m_indexBuffer->GetElementSize());
	m_vertexBufferSRV = m_resourceManager->CreateSRV(m_vertexBuffer, m_vertexBuffer->GetElementCount(), m_vertexBuffer->GetElementSize());
}

void Ideal::D3D12RayTracingRenderer::BuildAccelerationStructures()
{
	ResetCommandList();

	//-------BLAS-------//
	ComPtr<ID3D12Resource> BottomScratch;	// UAV
	BuildBottomLevelAccelerationStructure(BottomScratch);

	//-------BLAS-------//
	ComPtr<ID3D12Resource> TopScratch;	// UAV
	ComPtr<ID3D12Resource> instanceBuffer;	//Upload
	BuildTopLevelAccelerationStructure(m_scratch, instanceBuffer);

	m_commandLists[m_currentContextIndex]->Close();
	ID3D12CommandList* ppCommandLists[] = { m_commandLists[m_currentContextIndex].Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Fence();
	WaitForFenceValue(m_lastFenceValues[m_currentContextIndex]);
}

void Ideal::D3D12RayTracingRenderer::BuildBottomLevelAccelerationStructure(ComPtr<ID3D12Resource>& ScratchBuffer)
{
	//---------BLAS-----------//
	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.IndexBuffer = m_indexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexCount = m_indexBuffer->GetElementCount();
	geometryDesc.Triangles.IndexFormat = INDEX_FORMAT;
	geometryDesc.Triangles.Transform3x4 = 0;
	geometryDesc.Triangles.VertexFormat = VERTEX_FORMAT;
	geometryDesc.Triangles.VertexCount = m_vertexBuffer->GetElementCount();
	geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = m_vertexBuffer->GetElementSize();

	// 가속구조에 필요한 사이즈를 가져온다
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = {};
	bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	bottomLevelInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	bottomLevelInputs.NumDescs = 1;
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs = &geometryDesc;

	m_device->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
	Check(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	// 임시 스크래치 데이터
	AllocateUAVBuffer(m_device.Get(), bottomLevelPrebuildInfo.ScratchDataSizeInBytes, ScratchBuffer.GetAddressOf(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

	// BLAS 버퍼를 저장할 UAV를 할당
	D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	//AllocateUAVBuffer(m_device.Get(), bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_bottomLevelAccelerationStructure, initialResourceState, L"BottomLevelAccelerationStructure");
	m_bottomLevelAccelerationStructure = std::make_shared<Ideal::D3D12UAVBuffer>();
	m_bottomLevelAccelerationStructure->Create(m_device.Get(), bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, initialResourceState, L"BottomLevelAccelerationStructure");
	// 최종 빌드
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	{
		bottomLevelBuildDesc.Inputs = bottomLevelInputs;
		bottomLevelBuildDesc.ScratchAccelerationStructureData = ScratchBuffer->GetGPUVirtualAddress();
		bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	}

	m_commandLists[m_currentContextIndex]->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
	CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure->GetResource());
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &uavBarrier);
}

void Ideal::D3D12RayTracingRenderer::BuildTopLevelAccelerationStructure(ComPtr<ID3D12Resource>& Scratch, ComPtr<ID3D12Resource>& instanceBuffer, const Matrix& Transform /*= Matrix::Identity*/)
{
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS tlasFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	tlasFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.Flags = tlasFlags;
	topLevelInputs.NumDescs = 1;
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPreBuildInfo = {};
	m_device->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPreBuildInfo);
	Check(topLevelPreBuildInfo.ResultDataMaxSizeInBytes > 0);

	//---------Create Scratch Buffer----------//
	AllocateUAVBuffer(m_device.Get(), topLevelPreBuildInfo.ScratchDataSizeInBytes, Scratch.GetAddressOf(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

	//---------Create TLAS BUFFER----------//
	D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	m_topLevelAccelerationStructure = std::make_shared<Ideal::D3D12UAVBuffer>();
	m_topLevelAccelerationStructure->Create(m_device.Get(), topLevelPreBuildInfo.ResultDataMaxSizeInBytes, initialResourceState, L"TopLevelAccelerationStructure");

	//----------Instance Desc---------//
	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
	instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
	XMStoreFloat3x4(reinterpret_cast<DirectX::XMFLOAT3X4*>(instanceDesc.Transform), Transform); // identity
	instanceDesc.InstanceMask = 1;
	instanceDesc.AccelerationStructure = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	AllocateUploadBuffer(m_device.Get(), &instanceDesc, sizeof(instanceDesc), &instanceBuffer, L"InstanceDescs");

	//---------TLAS DESC----------//
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	{
		topLevelInputs.InstanceDescs = instanceBuffer->GetGPUVirtualAddress();
		topLevelBuildDesc.Inputs = topLevelInputs;
		topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
		topLevelBuildDesc.ScratchAccelerationStructureData = Scratch->GetGPUVirtualAddress();
	}

	m_commandLists[m_currentContextIndex]->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
	CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_topLevelAccelerationStructure->GetResource());
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &uavBarrier);
}

void Ideal::D3D12RayTracingRenderer::BuildShaderTables()
{
	void* rayGenShaderIdentifier;
	void* missShaderIdentifier;
	void* hitGroupShaderIdentifier;

	uint32 shaderIdentifierSize;

	// Get shader identifiers
	{
		ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
		Check(m_dxrStateObject.As(&stateObjectProperties));
		shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_raygenShaderName);
		missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_missShaderName);
		hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_hitGroupName);
	}
	// Ray gen shader table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::DXRShaderTable rayGenShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
		rayGenShaderTable.push_back(Ideal::DXRShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
		m_rayGenShaderTable = rayGenShaderTable.GetResource();
	}

	// Miss shader table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::DXRShaderTable missShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(Ideal::DXRShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource();
	}

	// Hit group shader table
	{
		RootArgumentTest rootArguments;

		// cb
		rootArguments.CB_Cube = m_cubeCB;

		// indices
		m_indexBufferShaderTableHandle = m_shaderTableHeap->Allocate(1);
		//m_indexBufferShaderTableHandle = m_shaderTableHeap->Allocate(1);
		m_device->CopyDescriptorsSimple(1, m_indexBufferShaderTableHandle.GetCpuHandle(), m_indexBufferSRV->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		rootArguments.SRV_Indices = m_indexBufferShaderTableHandle.GetGpuHandle();

		// vertices
		m_vertexBufferShaderTableHandle = m_shaderTableHeap->Allocate(1);
		//m_vertexBufferShaderTableHandle = m_shaderTableHeap->Allocate(1);
		m_device->CopyDescriptorsSimple(1, m_vertexBufferShaderTableHandle.GetCpuHandle(), m_vertexBufferSRV->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		rootArguments.SRV_Vertices = m_vertexBufferShaderTableHandle.GetGpuHandle();

		// diffuse
		m_textureShaderTableHandle = m_shaderTableHeap->Allocate(1);
		//m_textureShaderTableHandle = m_shaderTableHeap->Allocate(1);
		m_device->CopyDescriptorsSimple(1, m_textureShaderTableHandle.GetCpuHandle(), m_texture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		rootArguments.SRV_DiffuseTexture = m_textureShaderTableHandle.GetGpuHandle();

		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);
		Ideal::DXRShaderTable hitGroupShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
		hitGroupShaderTable.push_back(Ideal::DXRShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
		m_hitGroupShaderTable = hitGroupShaderTable.GetResource();
	}
}

void Ideal::D3D12RayTracingRenderer::CreateRayTracingOutputResources()
{
	DXGI_FORMAT format = m_renderTargets[0]->GetResource()->GetDesc().Format;

	CD3DX12_RESOURCE_DESC uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		m_width,
		m_height,
		1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	Check(m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&uavDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(m_raytracingOutput.GetAddressOf())),
		L"Failed to Create RayTracing Output!"
	);
	m_raytracingOutput->SetName(L"RayTracing_OutPut");
	//m_descriptorHeaps[m_currentContextIndex]->GetC

	m_raytacingOutputResourceUAVGpuDescriptorHandle = m_uavDescriptorHeap->Allocate();
	D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
	//m_raytracingOutputResourceUAVDescriptorHeapIndex = CD3DX12_GPU_DESCRIPTOR_HANDLE()
	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	m_device->CreateUnorderedAccessView(m_raytracingOutput.Get(), nullptr, &UAVDesc, m_raytacingOutputResourceUAVGpuDescriptorHandle.GetCpuHandle());
	//m_raytacingOutputResourceUAVGpuDescriptorHandle = m_raytacingOutputResourceUAVGpuDescriptorHandle.GetGpuHandle();
}

void Ideal::D3D12RayTracingRenderer::CreateDescriptorHeap()
{
	m_uavDescriptorHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
	//m_uavDescriptorHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
	m_uavDescriptorHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
	m_shaderTableHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
	//m_shaderTableHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, MAX_DRAW_COUNT_PER_FRAME);	// temp
	m_shaderTableHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_DRAW_COUNT_PER_FRAME);	// temp
}

void Ideal::D3D12RayTracingRenderer::DoRaytracing()
{
	ComPtr<ID3D12GraphicsCommandList4> commandlist = m_commandLists[m_currentContextIndex];

	auto DispatchRays = [&](auto* commandlist, auto* stateObject, D3D12_DISPATCH_RAYS_DESC* dispatchDesc)
		{
			dispatchDesc->HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
			dispatchDesc->HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
			dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;

			dispatchDesc->MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
			dispatchDesc->MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
			dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;

			dispatchDesc->RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
			dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;

			dispatchDesc->Width = m_width;
			dispatchDesc->Height = m_height;
			dispatchDesc->Depth = 1;
			commandlist->SetPipelineState1(stateObject);
			commandlist->DispatchRays(dispatchDesc);
		};

	commandlist->SetComputeRootSignature(m_raytracingGlobalRootSignature.Get());
	commandlist->SetDescriptorHeaps(1, m_descriptorHeaps[m_currentContextIndex]->GetDescriptorHeap().GetAddressOf());

	// 1번 parameter를 (parameter은 register를 t0으로 초기화 해줬음)
	commandlist->SetComputeRootShaderResourceView(1, m_topLevelAccelerationStructure->GetGPUVirtualAddress());

	// parameter0에 바인딩 : render target
	auto handle0 = m_descriptorHeaps[m_currentContextIndex]->Allocate(1);
	m_device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), m_raytacingOutputResourceUAVGpuDescriptorHandle.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandlist->SetComputeRootDescriptorTable(0, handle0.GetGpuHandle());

	// parameter2 에 바인딩 : SceneConstantSlot
	auto cb = m_cbAllocator[m_currentContextIndex]->Allocate(m_device.Get(), sizeof(m_sceneCB));
	memcpy(cb->SystemMemAddr, &m_sceneCB, sizeof(m_sceneCB));
	auto handle2 = m_descriptorHeaps[m_currentContextIndex]->Allocate(1);
	m_device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandlist->SetComputeRootDescriptorTable(2, handle2.GetGpuHandle());

	// Bind the hepas, 가속구조, dispatch rays
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};

	DispatchRays(commandlist.Get(), m_dxrStateObject.Get(), &dispatchDesc);
}

void Ideal::D3D12RayTracingRenderer::DoRaytracing4()
{
	ComPtr<ID3D12GraphicsCommandList4> commandlist = m_commandLists[m_currentContextIndex];

	auto DispatchRays = [&](auto* commandlist, auto* stateObject, D3D12_DISPATCH_RAYS_DESC* dispatchDesc)
		{
			
			dispatchDesc->HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
			dispatchDesc->HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
			dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;

			dispatchDesc->MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
			dispatchDesc->MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
			dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;

			dispatchDesc->RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
			dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;
			//dispatchDesc->RayGenerationShaderRecord.StartAddress = m_raytracingManager->GetRayGenShaderTable()->GetGPUVirtualAddress();
			//dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_raytracingManager->GetRayGenShaderTable()->GetDesc().Width;

			dispatchDesc->Width = m_width;
			dispatchDesc->Height = m_height;
			dispatchDesc->Depth = 1;
			commandlist->SetPipelineState1(stateObject);
			commandlist->DispatchRays(dispatchDesc);
		};

	commandlist->SetComputeRootSignature(m_raytracingGlobalRootSignature.Get());
	commandlist->SetDescriptorHeaps(1, m_descriptorHeaps[m_currentContextIndex]->GetDescriptorHeap().GetAddressOf());

	// 1번 parameter를 (parameter은 register를 t0으로 초기화 해줬음)
	commandlist->SetComputeRootShaderResourceView(1, m_raytracingManager->GetTLASResource()->GetGPUVirtualAddress());

	// parameter0에 바인딩 : render target
	auto handle0 = m_descriptorHeaps[m_currentContextIndex]->Allocate(1);
	m_device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), m_raytacingOutputResourceUAVGpuDescriptorHandle.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandlist->SetComputeRootDescriptorTable(0, handle0.GetGpuHandle());

	// parameter2 에 바인딩 : SceneConstantSlot
	auto cb = m_cbAllocator[m_currentContextIndex]->Allocate(m_device.Get(), sizeof(m_sceneCB));
	memcpy(cb->SystemMemAddr, &m_sceneCB, sizeof(m_sceneCB));
	auto handle2 = m_descriptorHeaps[m_currentContextIndex]->Allocate(1);
	m_device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandlist->SetComputeRootDescriptorTable(2, handle2.GetGpuHandle());

	// Bind the hepas, 가속구조, dispatch rays
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};

	DispatchRays(commandlist.Get(), m_dxrStateObject.Get(), &dispatchDesc);
}

void Ideal::D3D12RayTracingRenderer::DoRaytracing5()
{
	ComPtr<ID3D12GraphicsCommandList4> commandlist = m_commandLists[m_currentContextIndex];

	auto DispatchRays = [&](auto* commandlist, auto* stateObject, D3D12_DISPATCH_RAYS_DESC* dispatchDesc)
		{
			dispatchDesc->HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
			dispatchDesc->HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
			dispatchDesc->HitGroupTable.StrideInBytes = m_hitGroupShaderTableStrideInBytes;

			dispatchDesc->MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
			dispatchDesc->MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
			dispatchDesc->MissShaderTable.StrideInBytes = m_missShaderTableStrideInBytes;

			dispatchDesc->RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
			dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;

			dispatchDesc->Width = m_width;
			dispatchDesc->Height = m_height;
			dispatchDesc->Depth = 1;
			commandlist->SetPipelineState1(stateObject);
			commandlist->DispatchRays(dispatchDesc);
		};
	commandlist->SetComputeRootSignature(m_raytracingGlobalRootSignature.Get());
	commandlist->SetDescriptorHeaps(1, m_descriptorHeaps[m_currentContextIndex]->GetDescriptorHeap().GetAddressOf());
	//commandlist->SetDescriptorHeaps(1, m_shaderTableHeap->GetDescriptorHeap().GetAddressOf());
	// 1번 parameter를 (parameter은 register를 t0으로 초기화 해줬음)
	commandlist->SetComputeRootShaderResourceView(1, m_raytracingManager->GetTLASResource()->GetGPUVirtualAddress());

	// parameter0에 바인딩 : render target
	auto handle0 = m_descriptorHeaps[m_currentContextIndex]->Allocate(1);
	m_device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), m_raytacingOutputResourceUAVGpuDescriptorHandle.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandlist->SetComputeRootDescriptorTable(0, handle0.GetGpuHandle());

	// parameter2 에 바인딩 : SceneConstantSlot
	auto cb = m_cbAllocator[m_currentContextIndex]->Allocate(m_device.Get(), sizeof(m_sceneCB));
	memcpy(cb->SystemMemAddr, &m_sceneCB, sizeof(m_sceneCB));
	auto handle2 = m_descriptorHeaps[m_currentContextIndex]->Allocate(1);
	m_device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandlist->SetComputeRootDescriptorTable(2, handle2.GetGpuHandle());

	// Bind the hepas, 가속구조, dispatch rays
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};

	DispatchRays(commandlist.Get(), m_dxrStateObject.Get(), &dispatchDesc);
}

void Ideal::D3D12RayTracingRenderer::CopyRaytracingOutputToBackBuffer()
{
	ComPtr<ID3D12GraphicsCommandList4> commandlist = m_commandLists[m_currentContextIndex];
	std::shared_ptr<Ideal::D3D12Texture> renderTarget = m_renderTargets[m_frameIndex];

	CD3DX12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTarget->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		m_raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	commandlist->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);
	commandlist->CopyResource(renderTarget->GetResource(), m_raytracingOutput.Get());

	CD3DX12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTarget->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		//D3D12_RESOURCE_STATE_PRESENT
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		m_raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	commandlist->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
}

void Ideal::D3D12RayTracingRenderer::UpdateForSizeChange(uint32 Width, uint32 Height)
{
	float border = 0.1f;
	//border = 0.0f;
	if (m_width <= m_height)
	{
		m_rayGenCB.stencil =
		{
			-1 + border,
			-1 + border * m_aspectRatio,
			1.0f - border,
			1 - border * m_aspectRatio
		};
	}
	else
	{
		m_rayGenCB.stencil =
		{
			-1 + border / m_aspectRatio,
			-1 + border,
			1 - border / m_aspectRatio,
			1.f - border
		};
	}
}

void Ideal::D3D12RayTracingRenderer::UpdateAccelerationStructure()
{

	static float rad = 0.f;
	if (rad > 360.f) rad = 0.f;
	rad += 0.01f;
	Matrix mat = Matrix::Identity * Matrix::CreateRotationY(rad);
	// Matrix mat = Matrix::Identity;
	// instance를 다시 올릴 upload buffer의 핸들
	std::shared_ptr<Ideal::UploadBufferContainer> handle = m_BLASInstancePool[m_currentContextIndex]->Allocate();

	// upload 할 주소를 받고
	DXRInstanceDesc* ptr = (DXRInstanceDesc*)handle->SystemMemoryAddress;
	// 주소에 데이터 복사 하고
	ptr->SetTransform(mat);
	ptr->InstanceMask = 1;
	ptr->AccelerationStructure = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();

	/// 리빌드
	// BLAS

	// TLAS
	D3D12_GPU_VIRTUAL_ADDRESS instanceDescs = handle->GpuVirtualAddress;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	{
		topLevelBuildDesc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		topLevelBuildDesc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		topLevelBuildDesc.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		topLevelBuildDesc.Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
		topLevelBuildDesc.Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

		topLevelBuildDesc.Inputs.NumDescs = 1;	// 일단 한 개
		topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs;

		// 업데이트일 경우 처음 만드는게 아닐면 desc.sourceAS를 채워줘야한다.
		topLevelBuildDesc.SourceAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
		topLevelBuildDesc.ScratchAccelerationStructureData = m_scratch->GetGPUVirtualAddress();
		topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
	}
	m_commandLists[m_currentContextIndex]->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(m_topLevelAccelerationStructure->GetResource());
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &barrier);
}

void Ideal::D3D12RayTracingRenderer::InitRenderScene()
{
	//m_renderScene->Init(m_device, TODO, TODO);
}

void Ideal::D3D12RayTracingRenderer::TestDrawRenderScene()
{
	m_renderScene->Draw(m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex]);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerInit()
{
	m_raytracingManager = std::make_shared<Ideal::RaytracingManager>();
	m_raytracingManager->Init(m_device, m_resourceManager, m_myShader);

	// geometry
	//BLASGeometry geo1;
	//geo1.Name = L"g1";
	//geo1.VertexBuffer = m_vertexBuffer;
	//geo1.IndexBuffer = m_indexBuffer;
	//
	//BLASGeometry geo2;
	//geo2.Name = L"g2";
	//geo2.VertexBuffer = m_vertexBuffer;
	//geo2.IndexBuffer = m_indexBuffer;
	//
	//BLASData desc1;
	//desc1.Geometries.push_back(geo1);
	//desc1.Geometries.push_back(geo2);
	//
	//BLASData desc2;
	//desc2.Geometries.push_back(geo2);
	//
	//uint32 index1 = m_raytracingManager->AddBLASAndGetInstanceIndex(m_device, desc1.Geometries, L"BLAS1");
	//uint32 index2 = m_raytracingManager->AddBLASAndGetInstanceIndex(m_device, desc2.Geometries, L"BLAS2");

	ResetCommandList();

	m_raytracingManager->FinalCreate(m_device, m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex]);

	m_commandLists[m_currentContextIndex]->Close();
	ID3D12CommandList* ppCommandLists[] = { m_commandLists[m_currentContextIndex].Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Fence();
	WaitForFenceValue(m_lastFenceValues[m_currentContextIndex]);

	//Matrix trs = Matrix::Identity * Matrix::CreateTranslation(Vector3(5, 0, 0));
	//m_raytracingManager->SetGeometryTransformByIndex(index2, trs);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerUpdate()
{
	//Matrix mat = Matrix::Identity;
	//static float rot = 0.f;
	//rot += 0.005f;
	//mat *= Matrix::CreateRotationY(rot);
	//m_raytracingManager->SetGeometryTransformByIndex(2, mat);
	m_raytracingManager->UpdateAccelerationStructures(m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex]);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerAddObject(std::shared_ptr<Ideal::IdealStaticMeshObject> obj)
{
	ResetCommandList();

	m_staticMeshObject->AllocateBLASInstanceID(m_device, m_raytracingManager);
	m_raytracingManager->FinalCreate(m_device, m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex]);

	m_commandLists[m_currentContextIndex]->Close();
	ID3D12CommandList* ppCommandLists[] = { m_commandLists[m_currentContextIndex].Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//m_raytracingManager->BuildShaderTables(m_device, m_resourceManager);
	BuildShaders2();
	Fence();
	WaitForFenceValue(m_lastFenceValues[m_currentContextIndex]);
}

void Ideal::D3D12RayTracingRenderer::BuildShaders2()
{
	void* rayGenShaderIdentifier;
	void* missShaderIdentifier;
	void* hitGroupShaderIdentifier;

	uint32 shaderIdentifierSize;

	// Get shader identifiers
	{
		ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
		Check(m_dxrStateObject.As(&stateObjectProperties));
		shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_raygenShaderName);
		missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_missShaderName);
		hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_hitGroupName);
	}
	// Ray gen shader table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::DXRShaderTable rayGenShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
		rayGenShaderTable.push_back(Ideal::DXRShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
		m_rayGenShaderTable = rayGenShaderTable.GetResource();
	}

	// Miss shader table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::DXRShaderTable missShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(Ideal::DXRShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource();
		m_missShaderTableStrideInBytes = missShaderTable.GetShaderRecordSize();
	}

	// Hit group shader table
	{
		struct RootArgument
		{
			// Index
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_Indices;
			// Vertex
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_Vertices;
			// Diffuse Texture
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_DiffuseTexture;
		} rootArguments;

		//uint32 numShaderRecords = m_contributionToHitGroupIndexCount;
		uint32 numShaderRecords = 3;
		if (numShaderRecords == 0) numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize + sizeof(RootArgument);
		Ideal::DXRShaderTable hitGroupShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");

		const std::map<std::wstring, std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> BlASes = m_raytracingManager->GetBLASes();

		for (auto& blasPair : BlASes)
		{
			auto& blas = blasPair.second;
			const std::vector<BLASGeometry> blasGeometries = blas->GetGeometries();
			for (const BLASGeometry& blasGeometry : blasGeometries)
			{
				// TEMP : 아래에서 핸들같은 것들 따로 프리 안해주고 있긴한데 사라지면서 프리가 될 것
				Ideal::LocalRootSignature::RootArgument rootArguments;

				std::shared_ptr<Ideal::D3D12IndexBuffer> indexBuffer = blasGeometry.IndexBuffer;
				std::shared_ptr<Ideal::D3D12VertexBuffer> vertexBuffer = blasGeometry.VertexBuffer;

				auto h1 = m_resourceManager->CreateSRV(indexBuffer, indexBuffer->GetElementCount(), indexBuffer->GetElementSize());
				auto h2 = m_resourceManager->CreateSRV(vertexBuffer, vertexBuffer->GetElementCount(), vertexBuffer->GetElementSize());

				// Indices
				//auto indicesLocation = m_shaderTableHeap->Allocate(1);
				auto indicesLocation = m_shaderTableHeap->Allocate(1);
				m_device->CopyDescriptorsSimple(1, indicesLocation.GetCpuHandle(), h1->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				// vertices
				auto verticesLocation = m_shaderTableHeap->Allocate(1);
				m_device->CopyDescriptorsSimple(1, verticesLocation.GetCpuHandle(), h2->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				// diffuse
				auto textureLocation = m_shaderTableHeap->Allocate(1);
				uint64 notExist = (uint64)D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
				if (blasGeometry.DiffuseTexture.GetCpuHandle().ptr != notExist)
				{
					m_device->CopyDescriptorsSimple(1, textureLocation.GetCpuHandle(), blasGeometry.DiffuseTexture.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				rootArguments.SRV_Indices = indicesLocation.GetGpuHandle();
				rootArguments.SRV_Vertices = verticesLocation.GetGpuHandle();
				rootArguments.SRV_DiffuseTexture = textureLocation.GetGpuHandle();

				hitGroupShaderTable.push_back(Ideal::DXRShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));

				// temp:
				handles.push_back(indicesLocation);
				handles.push_back(verticesLocation);
				handles.push_back(textureLocation);
				srvs.push_back(h1);
				srvs.push_back(h2);
			}
		}
		m_hitGroupShaderTable = hitGroupShaderTable.GetResource();
		m_hitGroupShaderTableStrideInBytes = hitGroupShaderTable.GetShaderRecordSize();
	}
}

void Ideal::D3D12RayTracingRenderer::CopyShaderTableToMainDescriptor()
{

}
