#include "D3D12RayTracingRenderer.h"

#include <DirectXColors.h>
#include "Misc/Utils/PIX.h"
#include "Misc/Assimp/AssimpConverter.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

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
#include "GraphicsEngine/D3D12/D3D12DescriptorManager.h"
#include "GraphicsEngine/D3D12/DeferredDeleteManager.h"

#include "GraphicsEngine/Resource/Light/IdealDirectionalLight.h"
#include "GraphicsEngine/Resource/Light/IdealSpotLight.h"
#include "GraphicsEngine/Resource/Light/IdealPointLight.h"

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
	m_fenceValue(0),
	m_isEditor(EditorMode)
{
	m_aspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
}

Ideal::D3D12RayTracingRenderer::~D3D12RayTracingRenderer()
{
	Fence();
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		WaitForFenceValue(m_lastFenceValues[i]);
		m_deferredDeleteManager->DeleteDeferredResources(i);
	}
	m_skyBoxTexture.reset();

#ifdef _DEBUG
	if (m_isEditor)
	{
		m_imguiSRVHandle.Free();
		m_editorTexture->Release();
		m_editorTexture.reset();

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
#endif

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

	//---------------Create Managers---------------//
	m_deferredDeleteManager = std::make_shared<Ideal::DeferredDeleteManager>();

	m_resourceManager = std::make_shared<Ideal::ResourceManager>();
	m_resourceManager->Init(m_device);
	m_resourceManager->SetAssetPath(m_assetPath);
	m_resourceManager->SetModelPath(m_modelPath);
	m_resourceManager->SetTexturePath(m_texturePath);
	{
		std::shared_ptr<Ideal::ShaderManager> m_shaderManager = std::make_shared<Ideal::ShaderManager>();
		m_shaderManager->Init();
		m_shaderManager->AddIncludeDirectories(L"../Shaders/Raytracing/");

		// shader test
		//InitShader();
		//CompileShader2(L"../Shaders/Raytracing/Raytracing.hlsl", m_raygenShaderName, m_testBlob);
		//m_shaderManager->CompileShader(L"../Shaders/Raytracing/Raytracing.hlsl", L"lib_6_3", m_testBlob);
		m_shaderManager->CompileShaderAndSave(
			L"../Shaders/Raytracing/Raytracing.hlsl",
			L"../Shaders/Raytracing/",
			L"Raytracing",
			L"lib_6_3",
			m_testBlob
		);
		m_shaderManager->LoadShaderFile(L"../Shaders/Raytracing/Raytracing.shader", m_myShader);
	}

	{
		std::shared_ptr<Ideal::ShaderManager> m_shaderManager = std::make_shared<Ideal::ShaderManager>();
		m_shaderManager->Init();
		m_shaderManager->AddIncludeDirectories(L"../Shaders/Raytracing/");

		m_shaderManager->CompileShaderAndSave(
			L"../Shaders/Raytracing/CS_ComputeAnimationVertexBuffer.hlsl",
			L"../Shaders/Raytracing/",
			L"CS_ComputeAnimationVertexBuffer",
			L"cs_6_3",
			m_testBlob,
			L"ComputeSkinnedVertex",
			true
		);
		m_shaderManager->LoadShaderFile(L"../Shaders/Raytracing/CS_ComputeAnimationVertexBuffer.shader", m_animationShader);
	}


	//---------------Editor---------------//
#ifdef _DEBUG
	if (m_isEditor)
	{
		//------ImGuiSRVHeap------//
		m_imguiSRVHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
		m_imguiSRVHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 2 /*FONT*/ /*Main Camera*/);

		//---------------------Editor RTV-------------------------//
		m_editorRTVHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
		m_editorRTVHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);

		CreateEditorRTV(m_width, m_height);
		InitImGui();
	}
#endif
	m_sceneCB.CameraPos = Vector4(0.f);

	m_sceneCB.lightPos = Vector4(3.f, 1.8f, -3.f, 0.f);
	//m_sceneCB.lightAmbient = Vector4(0.5f, 0.5f, 0.5f, 1.f);
	m_sceneCB.lightAmbient = Vector4(0.2f, 0.2f, 0.2f, 1.f);
	//m_sceneCB.lightDiffuse = Vector4(0.5f, 0.f, 0.f, 1.f);
	m_sceneCB.lightDiffuse = Vector4(1.f, 1.f, 1.f, 1.f);

	m_sceneCB.maxRadianceRayRecursionDepth = G_MAX_RAY_RECURSION_DEPTH;
	m_sceneCB.maxShadowRayRecursionDepth = G_MAX_RAY_RECURSION_DEPTH;

	m_sceneCB.color = Vector4(1.f, 1.f, 1.f, 1.f);

	// load image

	// create resource
	//CreateDeviceDependentResources();

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
	//m_mainCamera->UpdateViewMatrix();

	m_sceneCB.CameraPos = m_mainCamera->GetPosition();
	m_sceneCB.ProjToWorld = m_mainCamera->GetViewProj().Invert().Transpose();
	UpdateLightListCBData();
	m_sceneCB.color = m_directionalLight->GetDirectionalLightDesc().DiffuseColor;
	//m_sceneCB.color = Vector4(1.f, 1.f, 1.f, 1.f);
	m_sceneCB.lightDiffuse = m_directionalLight->GetDirectionalLightDesc().DiffuseColor;
	ResetCommandList();

#ifdef _DEBUG
	if (m_isEditor)
	{
		/*SetImGuiCameraRenderTarget();
		DrawImGuiMainCamera();*/
	}
#endif

	BeginRender();

	//---------------------Raytracing-------------------------//
	for (auto& mesh : m_staticMeshObject)
	{
		mesh->UpdateBLASInstance(m_raytracingManager);
	}
	for (auto& mesh : m_skinnedMeshObject)
	{
		mesh->UpdateBLASInstance(m_device,
			m_commandLists[m_currentContextIndex],
			m_resourceManager,
			m_descriptorManager,
			m_currentContextIndex,
			m_cbAllocator[m_currentContextIndex],
			m_raytracingManager
		);
	}

	RaytracingManagerUpdate();

	m_raytracingManager->DispatchRays(
		m_device,
		m_commandLists[m_currentContextIndex],
		m_descriptorManager,
		m_currentContextIndex,
		m_cbAllocator[m_currentContextIndex],
		m_sceneCB, m_lightListCB, m_skyBoxTexture);

	CopyRaytracingOutputToBackBuffer();

#ifdef _DEBUG
	if (m_isEditor)
	{
		ComPtr<ID3D12GraphicsCommandList4> commandlist = m_commandLists[m_currentContextIndex];
		//std::shared_ptr<Ideal::D3D12Texture> renderTarget = m_renderTargets[m_frameIndex];
		ComPtr<ID3D12Resource> raytracingOutput = m_raytracingManager->GetRaytracingOutputResource();

		CD3DX12_RESOURCE_BARRIER preCopyBarriers[2];
		preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_editorTexture->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_DEST
		);
		preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutput.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COPY_SOURCE
		);
		commandlist->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);
		commandlist->CopyResource(m_editorTexture->GetResource(), raytracingOutput.Get());

		CD3DX12_RESOURCE_BARRIER postCopyBarriers[2];
		postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_editorTexture->GetResource(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutput.Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);

		commandlist->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
		DrawImGuiMainCamera();
	}
#endif

	//---------------------Editor-------------------------//
#ifdef _DEBUG
	if (m_isEditor)
	{
		ImGuiIO& io = ImGui::GetIO();
		ComPtr<ID3D12GraphicsCommandList> commandList = m_commandLists[m_currentContextIndex];
		//------IMGUI RENDER------//
		//ID3D12DescriptorHeap* descriptorHeap[] = { m_resourceManager->GetImguiSRVHeap().Get() };
		ID3D12DescriptorHeap* descriptorHeap[] = { m_imguiSRVHeap->GetDescriptorHeap().Get() };
		commandList->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(nullptr, (void*)commandList.Get());
		}
	}
#endif

	//---------------------Present-------------------------//
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
	newStaticMesh->SetName(FileName);
	//newStaticMesh->Init(m_device);

	// temp
	auto mesh = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(newStaticMesh);

	RaytracingManagerAddObject(mesh);

	m_staticMeshObject.push_back(mesh);
	return newStaticMesh;
	//return nullptr;
}

std::shared_ptr<Ideal::ISkinnedMeshObject> Ideal::D3D12RayTracingRenderer::CreateSkinnedMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealSkinnedMeshObject> newSkinnedMesh = std::make_shared<Ideal::IdealSkinnedMeshObject>();
	m_resourceManager->CreateSkinnedMeshObject(newSkinnedMesh, FileName);
	newSkinnedMesh->SetName(FileName);

	auto mesh = std::static_pointer_cast<Ideal::IdealSkinnedMeshObject>(newSkinnedMesh);
	RaytracingManagerAddObject(mesh);
	m_skinnedMeshObject.push_back(mesh);
	// 인터페이스로 따로 뽑아야 할 듯
	return newSkinnedMesh;
}

void Ideal::D3D12RayTracingRenderer::DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> MeshObject)
{
	if (MeshObject == nullptr)
		return;

	if (MeshObject->GetMeshType() == Ideal::Static)
	{
		auto mesh = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject);
		RaytracingManagerDeleteObject(mesh);

		auto it = std::find(m_staticMeshObject.begin(), m_staticMeshObject.end(), mesh);
		{
			*it = std::move(m_staticMeshObject.back());
			m_deferredDeleteManager->AddMeshObjectToDeferredDelete(MeshObject);
			m_staticMeshObject.pop_back();
		}
	}
	else if (MeshObject->GetMeshType() == Ideal::Skinned)
	{
		auto mesh = std::static_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject);
		RaytracingManagerDeleteObject(mesh);

		auto it = std::find(m_skinnedMeshObject.begin(), m_skinnedMeshObject.end(), mesh);
		{
			*it = std::move(m_skinnedMeshObject.back());
			m_deferredDeleteManager->AddMeshObjectToDeferredDelete(MeshObject);
			m_skinnedMeshObject.pop_back();
		}
	}
}

void Ideal::D3D12RayTracingRenderer::DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> DebugMeshObject)
{
	// 아직 디버그 매쉬를 안만들고 static mesh에서 그냥 만드니 여기서 삭제
	auto mesh = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(DebugMeshObject);
	RaytracingManagerDeleteObject(mesh);

	auto it = std::find(m_staticMeshObject.begin(), m_staticMeshObject.end(), mesh);
	{
		*it = std::move(m_staticMeshObject.back());
		m_deferredDeleteManager->AddMeshObjectToDeferredDelete(DebugMeshObject);
		m_staticMeshObject.pop_back();
	}
}

std::shared_ptr<Ideal::IMeshObject> Ideal::D3D12RayTracingRenderer::CreateDebugMeshObject(const std::wstring& FileName)
{
	// 작동안함
	return CreateStaticMeshObject(FileName);
}

std::shared_ptr<Ideal::IAnimation> Ideal::D3D12RayTracingRenderer::CreateAnimation(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealAnimation> newAnimation = std::make_shared<Ideal::IdealAnimation>();
	m_resourceManager->CreateAnimation(newAnimation, FileName);

	return newAnimation;
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
	std::shared_ptr<Ideal::IDirectionalLight> newLight = std::make_shared<Ideal::IdealDirectionalLight>();
	m_directionalLight = std::static_pointer_cast<Ideal::IdealDirectionalLight>(newLight);
	return newLight;
}

std::shared_ptr<Ideal::ISpotLight> Ideal::D3D12RayTracingRenderer::CreateSpotLight()
{
	std::shared_ptr<Ideal::ISpotLight> newLight = std::make_shared<Ideal::IdealSpotLight>();
	m_spotLights.push_back(std::static_pointer_cast<Ideal::IdealSpotLight>(newLight));
	return newLight;
}

std::shared_ptr<Ideal::IPointLight> Ideal::D3D12RayTracingRenderer::CreatePointLight()
{
	std::shared_ptr<Ideal::IPointLight> newLight = std::make_shared<Ideal::IdealPointLight>();
	m_pointLights.push_back(std::static_pointer_cast<Ideal::IdealPointLight>(newLight));
	return newLight;
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
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName, isSkinnedData, NeedVertexInfo);

	// Temp : ".fbx" 삭제
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();

	assimpConverter->ExportModelData(FileName, isSkinnedData);
	if (NeedVertexInfo)
	{
		assimpConverter->ExportVertexPositionData(FileName);
	}
	assimpConverter->ExportMaterialData(FileName);
}

void Ideal::D3D12RayTracingRenderer::ConvertAnimationAssetToMyFormat(std::wstring FileName)
{
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName, false, false);

	// Temp : ".fbx" 삭제
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();

	assimpConverter->ExportAnimationData(FileName);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool Ideal::D3D12RayTracingRenderer::SetImGuiWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (::ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
}

void Ideal::D3D12RayTracingRenderer::ClearImGui()
{
#ifdef _DEBUG
	if (m_isEditor)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();
	}
#endif
}

void Ideal::D3D12RayTracingRenderer::SetSkyBox(const std::wstring& FileName)
{
	std::shared_ptr <Ideal::D3D12Texture> skyBox = std::make_shared<Ideal::D3D12Texture>();
	m_resourceManager->CreateTextureDDS(skyBox, FileName);
	m_skyBoxTexture = skyBox;
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
		//m_BLASInstancePool[i]->Init(m_device.Get(), sizeof(Ideal::DXRInstanceDesc), MAX_DRAW_COUNT_PER_FRAME);
		m_BLASInstancePool[i]->Init(m_device.Get(), sizeof(Ideal::DXRInstanceDesc), MAX_DRAW_COUNT_PER_FRAME);
	}

	m_descriptorManager = std::make_shared<Ideal::D3D12DescriptorManager>();
	m_descriptorManager->Create(m_device, MAX_PENDING_FRAME_COUNT, MAX_DRAW_COUNT_PER_FRAME, 1, MAX_DRAW_COUNT_PER_FRAME * 4);
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
	//hr = m_swapChain->Present(1, 0);
	//hr = m_swapChain->Present(0, 0);
	Check(hr);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	uint32 nextContextIndex = (m_currentContextIndex + 1) % MAX_PENDING_FRAME_COUNT;
	WaitForFenceValue(m_lastFenceValues[nextContextIndex]);

	m_descriptorHeaps[nextContextIndex]->Reset();
	m_cbAllocator[nextContextIndex]->Reset();
	//m_BLASInstancePool[nextContextIndex]->Reset();
	m_descriptorManager->ResetPool(nextContextIndex);

	// deferred resource Delete And Set Next Context Index
	m_deferredDeleteManager->DeleteDeferredResources(m_currentContextIndex);

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
	uint64 completedValue = m_fence->GetCompletedValue();
	if (m_fence->GetCompletedValue() < ExpectedFenceValue)
	{
		m_fence->SetEventOnCompletion(ExpectedFenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
	uint64 completedValue1 = m_fence->GetCompletedValue();
	int a = 3;
}

void Ideal::D3D12RayTracingRenderer::UpdateLightListCBData()
{
	//----------------Directional Light-----------------//
	if (m_directionalLight)
	{
		m_lightListCB.DirLight = m_directionalLight->GetDirectionalLightDesc();
		//m_lightListCB.DirLight.AmbientColor = m_directionalLight->GetDirectionalLightDesc().AmbientColor;
		//m_lightListCB.DirLight.DiffuseColor = m_directionalLight->GetDirectionalLightDesc().DiffuseColor;
		//m_lightListCB.DirLight.Direction = m_directionalLight->GetDirectionalLightDesc().Direction;
	}

	//----------------Spot Light-----------------//
	uint32 spotLightNum = m_spotLights.size();

	if (spotLightNum > MAX_SPOT_LIGHT_NUM)
	{
		spotLightNum = MAX_SPOT_LIGHT_NUM;
	}

	for (uint32 i = 0; i < spotLightNum; ++i)
	{
		m_lightListCB.SpotLights[i] = m_spotLights[i]->GetSpotLightDesc();
	}
	m_lightListCB.SpotLightNum = spotLightNum;

	//----------------Point Light-----------------//
	uint32 pointLightNum = m_pointLights.size();

	if (pointLightNum > MAX_POINT_LIGHT_NUM)
	{
		pointLightNum = MAX_POINT_LIGHT_NUM;
	}
	m_lightListCB.PointLightNum = pointLightNum;

	for (uint32 i = 0; i < pointLightNum; ++i)
	{
		m_lightListCB.PointLights[i] = m_pointLights[i]->GetPointLightDesc();
	}
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

void Ideal::D3D12RayTracingRenderer::CopyRaytracingOutputToBackBuffer()
{
	ComPtr<ID3D12GraphicsCommandList4> commandlist = m_commandLists[m_currentContextIndex];
	std::shared_ptr<Ideal::D3D12Texture> renderTarget = m_renderTargets[m_frameIndex];
	ComPtr<ID3D12Resource> raytracingOutput = m_raytracingManager->GetRaytracingOutputResource();

	CD3DX12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTarget->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	commandlist->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);
	commandlist->CopyResource(renderTarget->GetResource(), raytracingOutput.Get());

	CD3DX12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTarget->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	commandlist->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
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
	m_raytracingManager->Init(m_device, m_resourceManager, m_myShader, m_animationShader, m_descriptorManager, m_width, m_height);

	//ResetCommandList();

	m_raytracingManager->FinalCreate2(m_device, m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex], true);

	//m_commandLists[m_currentContextIndex]->Close();
	//ID3D12CommandList* ppCommandLists[] = { m_commandLists[m_currentContextIndex].Get() };
	//m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	//
	//Fence();
	//WaitForFenceValue(m_lastFenceValues[m_currentContextIndex]);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerUpdate()
{
	m_raytracingManager->UpdateAccelerationStructures(m_device, m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex], m_deferredDeleteManager);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerAddObject(std::shared_ptr<Ideal::IdealStaticMeshObject> obj)
{
	//ResetCommandList();
	auto blas = m_raytracingManager->GetBLASByName(obj->GetName().c_str());
	//std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = nullptr;
	bool ShouldBuildShaderTable = true;

	if (blas != nullptr)
	{
		obj->SetBLAS(blas);
		blas->AddRefCount();
		ShouldBuildShaderTable = false;
	}
	else
	{
		// 안에서 add ref count를 실행시키긴 함. ....
		blas = m_raytracingManager->AddBLAS(shared_from_this(), m_device, m_resourceManager, m_descriptorManager, m_cbAllocator[m_currentContextIndex], obj, obj->GetName().c_str(), false);
	}

	if (ShouldBuildShaderTable)
	{
		m_raytracingManager->BuildShaderTables(m_device, m_resourceManager, m_descriptorManager, m_deferredDeleteManager);
	}

	auto instanceDesc = m_raytracingManager->AllocateInstanceByBLAS(blas);
	obj->SetBLASInstanceDesc(instanceDesc);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerAddObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> obj)
{
	//ResetCommandList();
	auto blas = m_raytracingManager->AddBLAS(shared_from_this(), m_device, m_resourceManager, m_descriptorManager, m_cbAllocator[m_currentContextIndex], obj, obj->GetName().c_str(), true);
	// Skinning 데이터는 쉐이더 테이블을 그냥 만든다.
	m_raytracingManager->BuildShaderTables(m_device, m_resourceManager, m_descriptorManager, m_deferredDeleteManager);

	auto instanceDesc = m_raytracingManager->AllocateInstanceByBLAS(blas);
	obj->SetBLASInstanceDesc(instanceDesc);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerDeleteObject(std::shared_ptr<Ideal::IdealStaticMeshObject> obj)
{
	const std::wstring& name = obj->GetName();
	auto blas = obj->GetBLAS();
	auto blasInstance = obj->GetBLASInstanceDesc();
	m_raytracingManager->DeleteBLASInstance(blasInstance);
	if (blas != nullptr)
	{
		bool ShouldDeleteBLAS = blas->SubRefCount();
		if (ShouldDeleteBLAS)
		{
			//m_deferredDeleteManager->AddD3D12ResourceToDelete(blas->GetResource());
			m_deferredDeleteManager->AddBLASToDeferredDelete(blas);
			m_raytracingManager->DeleteBLAS(blas, name, false);

			//m_raytracingManager->BuildShaderTables(m_device, m_resourceManager, m_descriptorManager, m_deferredDeleteManager);
		}
	}
	//obj.reset();

}

void Ideal::D3D12RayTracingRenderer::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	//-----Allocate Srv-----//
	m_imguiSRVHandle = m_imguiSRVHeap->Allocate();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX12_Init(m_device.Get(), SWAP_CHAIN_FRAME_COUNT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		m_imguiSRVHeap->GetDescriptorHeap().Get(),
		m_imguiSRVHandle.GetCpuHandle(),
		m_imguiSRVHandle.GetGpuHandle());
}

void Ideal::D3D12RayTracingRenderer::DrawImGuiMainCamera()
{
	ImGui::Begin("MAIN SCREEN");                          // Create a window called "Hello, world!" and append into it.

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 size(windowSize.x, windowSize.y);
	//ImVec2 size(m_width/4, m_height/4);

	//m_width = windowSize.x;
	//m_height = windowSize.y;
	m_aspectRatio = float(windowSize.x) / windowSize.y;
	m_mainCamera->SetAspectRatio(m_aspectRatio);


	// to srv
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_editorTexture->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &barrier);

	ImGui::Image((ImTextureID)(m_editorTexture->GetSRV().GetGpuHandle().ptr), size);
	// to present
	/*barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_editorRenderTarget->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &barrier);*/

	ImGui::End();
}

void Ideal::D3D12RayTracingRenderer::SetImGuiCameraRenderTarget()
{
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandAllocators[m_currentContextIndex];
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandLists[m_currentContextIndex];

	CD3DX12_RESOURCE_BARRIER editorBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_editorTexture->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &editorBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	auto rtvHandle = m_editorTexture->GetRTV();


	commandList->ClearRenderTargetView(rtvHandle.GetCpuHandle(), DirectX::Colors::Black, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->RSSetViewports(1, &m_viewport->GetViewport());
	commandList->RSSetScissorRects(1, &m_viewport->GetScissorRect());
	commandList->OMSetRenderTargets(1, &rtvHandle.GetCpuHandle(), FALSE, &dsvHandle);
}

void Ideal::D3D12RayTracingRenderer::CreateEditorRTV(uint32 Width, uint32 Height)
{
	m_editorTexture = std::make_shared<Ideal::D3D12Texture>();
	{
		{
			ComPtr<ID3D12Resource> resource;
			CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
				DXGI_FORMAT_R8G8B8A8_UNORM,
				Width,
				Height, 1, 1
			);
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			//------Create------//
			Check(m_device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				//D3D12_RESOURCE_STATE_COMMON,
				//nullptr,
				nullptr,
				IID_PPV_ARGS(resource.GetAddressOf())
			));
			m_editorTexture->Create(resource);

			//-----SRV-----//
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = resource->GetDesc().Format;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				Ideal::D3D12DescriptorHandle srvHandle = m_imguiSRVHeap->Allocate();
				m_device->CreateShaderResourceView(resource.Get(), &srvDesc, srvHandle.GetCpuHandle());
				m_editorTexture->EmplaceSRV(srvHandle);
			}
			//-----RTV-----//
			{
				D3D12_RENDER_TARGET_VIEW_DESC RTVDesc{};
				RTVDesc.Format = resource->GetDesc().Format;
				RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				RTVDesc.Texture2D.MipSlice = 0;

				Ideal::D3D12DescriptorHandle rtvHandle = m_editorRTVHeap->Allocate();
				m_device->CreateRenderTargetView(resource.Get(), &RTVDesc, rtvHandle.GetCpuHandle());
				m_editorTexture->EmplaceRTV(rtvHandle);
			}
			m_editorTexture->GetResource()->SetName(L"Editor Texture");
		}
	}
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerDeleteObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> obj)
{
	const std::wstring& name = obj->GetName();
	auto blas = obj->GetBLAS();
	//auto blas = m_raytracingManager->GetBLASByName(name);
	auto blasInstance = obj->GetBLASInstanceDesc();
	m_raytracingManager->DeleteBLASInstance(blasInstance);
	if (blas != nullptr)
	{
		bool ShouldDeleteBLAS = blas->SubRefCount();
		if (ShouldDeleteBLAS)
		{
			//m_deferredDeleteManager->AddD3D12ResourceToDelete(blas->GetResource());
			m_deferredDeleteManager->AddBLASToDeferredDelete(blas);
			m_raytracingManager->DeleteBLAS(blas, name, true);
		}
	}
	//obj.reset();
	//m_raytracingManager->BuildShaderTables(m_device, m_resourceManager, m_descriptorManager, m_deferredDeleteManager);
}