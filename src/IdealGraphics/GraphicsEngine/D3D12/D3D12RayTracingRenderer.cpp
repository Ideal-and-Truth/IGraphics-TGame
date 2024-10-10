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
#include "GraphicsEngine/D3D12/D3D12UAV.h"
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
#include "GraphicsEngine/Resource/UI/IdealCanvas.h"
#include "GraphicsEngine/Resource/UI/IdealText.h"

#include "GraphicsEngine/Resource/Particle/ParticleSystemManager.h"
#include "GraphicsEngine/Resource/Particle/ParticleSystem.h"
#include "GraphicsEngine/Resource/Particle/ParticleMaterial.h"

#include "GraphicsEngine/Resource/DebugMesh/DebugMeshManager.h"

#include "GraphicsEngine/Resource/Light/IdealDirectionalLight.h"
#include "GraphicsEngine/Resource/Light/IdealSpotLight.h"
#include "GraphicsEngine/Resource/Light/IdealPointLight.h"

#include "GraphicsEngine/Resource/IdealMaterial.h"
#include "GraphicsEngine/Resource/UI/IdealSprite.h"

#include "GraphicsEngine/D3D12/TestShader.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"

#include "GraphicsEngine/D2D/D2DTextManager.h"

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

const Ideal::DisplayResolution Ideal::D3D12RayTracingRenderer::m_resolutionOptions[] =
{
	{ 800u, 600u },
	//{ 1280u, 960u },
	{ 1200u, 900u },
	{ 1280u, 720u },
	{ 1920u, 1080u },
	{ 1920u, 1200u },
	{ 2560u, 1440u },
	{ 3440u, 1440u },
	{ 3840u, 2160u }
};

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
	// 최적화 되었던 오브젝트 삭제
	ReleaseBakedObject();

	Fence();
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		WaitForFenceValue(m_lastFenceValues[i]);
		m_deferredDeleteManager->DeleteDeferredResources(i);
	}

	if (m_tearingSupport)
	{
		Check(m_swapChain->SetFullscreenState(FALSE, nullptr));
	}

	m_skyBoxTexture.reset();

	if (m_isEditor)
	{
		m_imguiSRVHandle.Free();
		m_editorTexture->Release();
		m_editorTexture.reset();

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	m_resourceManager->GetDefaultMaterial()->FreeInRayTracing();
	m_raytracingManager = nullptr;
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
	m_viewport = std::make_shared<Ideal::D3D12Viewport>(m_width, m_height);
	m_viewport->Init();

	m_postViewport = std::make_shared<Ideal::D3D12Viewport>(m_width, m_height);
	m_postViewport->Init();

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

	CreatePostScreenRootSignature();
	CreatePostScreenPipelineState();

	//------------------UI-------------------//
	CreateUIDescriptorHeap();
	CreateCanvas();

	//------------------Particle System Manager-------------------//
	CreateParticleSystemManager();


	//---------------Create Managers---------------//
	CompileShader(L"../Shaders/Texture/CS_GenerateMips.hlsl", L"../Shaders/Texture/", L"GenerateMipsCS", L"cs_6_3", L"Main", L"../Shaders/Texture/");

	m_deferredDeleteManager = std::make_shared<Ideal::DeferredDeleteManager>();

	m_resourceManager = std::make_shared<Ideal::ResourceManager>();
	m_resourceManager->Init(m_device, m_deferredDeleteManager);
	m_resourceManager->SetAssetPath(m_assetPath);
	m_resourceManager->SetModelPath(m_modelPath);
	m_resourceManager->SetTexturePath(m_texturePath);
	m_resourceManager->CreateDefaultTextures();
	m_resourceManager->CreateDefaultMaterial();

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

	m_textManager = std::make_shared<Ideal::D2DTextManager>();
	m_textManager->Init(m_device, m_commandQueue);


	//------------------Debug Mesh Manager-------------------//
	if (m_isEditor)
	{
		CreateDebugMeshManager();
	}
	//----CompileShader----//
	InitModifyVertexBufferShader();
	CreateModifyVertexBufferRootSignature();
	CreateModifyVertexBufferCSPipelineState();

	//---------------Editor---------------//
	if (m_isEditor)
	{
		//------ImGuiSRVHeap------//
		m_imguiSRVHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
		m_imguiSRVHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_EDITOR_SRV_COUNT);

		//---------------------Editor RTV-------------------------//
		m_editorRTVHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
		m_editorRTVHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);

		CreateEditorRTV(m_width, m_height);
		InitImGui();

		if (m_isEditor)
		{
			{
				auto srv = m_resourceManager->GetDefaultAlbedoTexture()->GetSRV();
				auto dest = m_imguiSRVHeap->Allocate();
				m_device->CopyDescriptorsSimple(1, dest.GetCpuHandle(), srv.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				m_resourceManager->GetDefaultAlbedoTexture()->EmplaceSRVInEditor(dest);
			}
			{
				auto srv = m_resourceManager->GetDefaultNormalTexture()->GetSRV();
				auto dest = m_imguiSRVHeap->Allocate();
				m_device->CopyDescriptorsSimple(1, dest.GetCpuHandle(), srv.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				m_resourceManager->GetDefaultNormalTexture()->EmplaceSRVInEditor(dest);
			}
			{
				auto srv = m_resourceManager->GetDefaultMaskTexture()->GetSRV();
				auto dest = m_imguiSRVHeap->Allocate();
				m_device->CopyDescriptorsSimple(1, dest.GetCpuHandle(), srv.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				m_resourceManager->GetDefaultMaskTexture()->EmplaceSRVInEditor(dest);
			}
		}
	}
	m_sceneCB.CameraPos = Vector4(0.f);

	m_sceneCB.maxRadianceRayRecursionDepth = G_MAX_RAY_RECURSION_DEPTH;
	m_sceneCB.maxShadowRayRecursionDepth = G_MAX_RAY_RECURSION_DEPTH;

	m_sceneCB.nearZ = 0;
	m_sceneCB.farZ = 0;
	//m_sceneCB.nearZ = m_mainCamera->GetNearZ();
	//m_sceneCB.farZ = m_mainCamera->GetFarZ();

	// load image

	// create resource
	//CreateDeviceDependentResources();

	RaytracingManagerInit();
	m_raytracingManager->CreateMaterialInRayTracing(m_device, m_descriptorManager, m_resourceManager->GetDefaultMaterial());


	// shader compile


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
	m_sceneCB.View = m_mainCamera->GetView().Transpose();
	m_sceneCB.Proj = m_mainCamera->GetProj().Transpose();
	m_sceneCB.nearZ = m_mainCamera->GetNearZ();
	m_sceneCB.farZ = m_mainCamera->GetFarZ();

	m_globalCB.View = m_mainCamera->GetView().Transpose();
	m_globalCB.Proj = m_mainCamera->GetProj().Transpose();
	m_globalCB.ViewProj = m_mainCamera->GetViewProj().Transpose();
	m_globalCB.eyePos = m_mainCamera->GetPosition();

	UpdateLightListCBData();
	if (m_directionalLight)
	{
		//m_sceneCB.color = m_directionalLight->GetDirectionalLightDesc().DiffuseColor;
		//m_sceneCB.color = Vector4(1.f, 1.f, 1.f, 1.f);
		//m_sceneCB.lightDiffuse = m_directionalLight->GetDirectionalLightDesc().DiffuseColor;
	}
	ResetCommandList();

#ifdef _DEBUG
	if (m_isEditor)
	{
		/*SetImGuiCameraRenderTarget();
		DrawImGuiMainCamera();*/
	}
#endif

	BeginRender();

	if (m_ReBuildBLASFlag)
	{
		ReBuildBLAS();
	}

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
		m_sceneCB, &m_lightListCB, m_skyBoxTexture);

#ifdef BeforeRefactor
	CopyRaytracingOutputToBackBuffer();
#else
	TransitionRayTracingOutputToRTV();
#endif
	//---------Particle---------//
	DrawParticle();

	//----Debug Mesh Draw----//
	if (m_isEditor)
	{
		DrawDebugMeshes();
	}

	//-----------UI-----------//
	// Update Text Or Dynamic Texture 
	// Draw Text and Texture
	DrawCanvas();
#ifndef BeforeRefactor
	TransitionRayTracingOutputToSRV();
	// Final
	DrawPostScreen();
#endif
	//TransitionRayTracingOutputToUAV();
	//CopyRaytracingOutputToBackBuffer();

	if (m_isEditor)
	{
		ComPtr<ID3D12GraphicsCommandList4> commandlist = m_commandLists[m_currentContextIndex];
		ComPtr<ID3D12Resource> raytracingOutput = m_renderTargets[m_frameIndex]->GetResource();


		CD3DX12_RESOURCE_BARRIER preCopyBarriers[2];
		preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_editorTexture->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_DEST
		);
		preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutput.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_SOURCE
		);
		commandlist->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);
		commandlist->CopyResource(m_editorTexture->GetResource(), raytracingOutput.Get());

		CD3DX12_RESOURCE_BARRIER postCopyBarriers[2];
		postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
			m_editorTexture->GetResource(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutput.Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		commandlist->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
		DrawImGuiMainCamera();
	}

	//---------------------Editor-------------------------//
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

	//---------------------Present-------------------------//
	EndRender();
	Present();
	return;
}

void Ideal::D3D12RayTracingRenderer::Resize(UINT Width, UINT Height)
{
	if (!(Width * Height))
		return;
	if (m_width == Width && m_height == Height)
		return;

	// Wait For All Commands
	Fence();
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		WaitForFenceValue(m_lastFenceValues[i]);
	}
	DXGI_SWAP_CHAIN_DESC1 desc;
	HRESULT hr = m_swapChain->GetDesc1(&desc);
	for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		m_renderTargets[i]->Release();
	}

	// ResizeBuffers
	Check(m_swapChain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, Width, Height, DXGI_FORMAT_R8G8B8A8_UNORM, m_swapChainFlags));

	BOOL isFullScreenState = false;
	Check(m_swapChain->GetFullscreenState(&isFullScreenState, nullptr));
	m_windowMode = !isFullScreenState;

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		auto handle = m_renderTargets[i]->GetRTV();
		ComPtr<ID3D12Resource> resource = m_renderTargets[i]->GetResource();
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&resource));
		m_device->CreateRenderTargetView(resource.Get(), nullptr, handle.GetCpuHandle());

		m_renderTargets[i]->Create(resource, m_deferredDeleteManager);
		m_renderTargets[i]->EmplaceRTV(handle);
	}

	// CreateDepthStencil
	//CreateDSV(Width, Height);

	m_width = Width;
	m_height = Height;

	// Viewport Reize
	m_viewport->ReSize(Width, Height);
	m_postViewport->ReSize(Width, Height);
	{
		auto r = m_resolutionOptions[m_displayResolutionIndex];
		m_postViewport->UpdatePostViewAndScissor(r.Width, r.Height);
	}
	//m_postViewport->UpdatePostViewAndScissor(Width, Height);

	if (m_mainCamera)
	{
		auto r = m_resolutionOptions[m_displayResolutionIndex];
		m_mainCamera->SetAspectRatio(float(r.Width) / r.Height);
	}

	if (m_isEditor)
	{
		//auto r = m_resolutionOptions[m_displayResolutionIndex];
		CreateEditorRTV(Width, Height);
	}

	// ray tracing / UI //
	//m_raytracingManager->Resize(m_device, Width, Height);
	m_UICanvas->SetCanvasSize(Width, Height);
	//
}
void Ideal::D3D12RayTracingRenderer::ToggleFullScreenWindow()
{
	if (m_fullScreenMode)
	{
		SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(
			m_hwnd,
			HWND_NOTOPMOST,
			m_windowRect.left, m_windowRect.top, m_windowRect.right - m_windowRect.left, m_windowRect.bottom - m_windowRect.top, SWP_FRAMECHANGED | SWP_NOACTIVATE
		);
		ShowWindow(m_hwnd, SW_NORMAL);
	}
	else
	{
		// 기존 윈도우의 Rect를 저장하여 전체화면 모드를 빠져나올 때 돌아올 수 있다.
		GetWindowRect(m_hwnd, &m_windowRect);

		// 윈도우를 경계없이 만들어서 클라이언트가 화면 전체를 채울 수 있게 한다.
		SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

		RECT fullScreenWindowRect;
		if (m_swapChain)
		{
			ComPtr<IDXGIOutput> output;
			Check(m_swapChain->GetContainingOutput(&output));
			DXGI_OUTPUT_DESC Desc;
			Check(output->GetDesc(&Desc));
			fullScreenWindowRect = Desc.DesktopCoordinates;
		}
		else
		{
			DEVMODE devMode = {};
			devMode.dmSize = sizeof(DEVMODE);
			EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

			fullScreenWindowRect =
			{
				devMode.dmPosition.x,
				devMode.dmPosition.y,
				devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
				devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
			};
		}

		SetWindowPos(
			m_hwnd,
			HWND_TOPMOST,
			fullScreenWindowRect.left,
			fullScreenWindowRect.top,
			fullScreenWindowRect.right,
			fullScreenWindowRect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(m_hwnd, SW_MAXIMIZE);
	}

	m_fullScreenMode = !m_fullScreenMode;
}

bool Ideal::D3D12RayTracingRenderer::IsFullScreen()
{
	return m_fullScreenMode;
}

void Ideal::D3D12RayTracingRenderer::SetDisplayResolutionOption(const Resolution::EDisplayResolutionOption& Resolution)
{
	m_displayResolutionIndex = Resolution;
	// Wait For All Commands
	Fence();
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		WaitForFenceValue(m_lastFenceValues[i]);
	}

	uint32 resolutionWidth = m_resolutionOptions[Resolution].Width;
	uint32 resolutionHeight = m_resolutionOptions[Resolution].Height;

	m_viewport->ReSize(resolutionWidth, resolutionHeight);
	m_postViewport->UpdatePostViewAndScissor(resolutionWidth, resolutionHeight);

	if (m_mainCamera)
	{
		m_mainCamera->SetAspectRatio(float(resolutionWidth) / resolutionHeight);
	}

	if (m_isEditor)
	{
		//CreateEditorRTV(resolutionWidth, resolutionHeight);
	}

	// ray tracing / UI //
	//m_depthStencil.Reset();
	CreateDSV(resolutionWidth, resolutionHeight);
	m_raytracingManager->Resize(m_resourceManager, m_device, resolutionWidth, resolutionHeight);
	m_UICanvas->SetCanvasSize(resolutionWidth, resolutionHeight);
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
	m_resourceManager->CreateStaticMeshObject(newStaticMesh, FileName, false);
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
		m_resourceManager->DeleteStaticMeshObject(mesh);
		auto it = std::find(m_staticMeshObject.begin(), m_staticMeshObject.end(), mesh);
		{
			if (it != m_staticMeshObject.end())
			{
				*it = std::move(m_staticMeshObject.back());
				m_deferredDeleteManager->AddMeshObjectToDeferredDelete(MeshObject);
				m_staticMeshObject.pop_back();
			}
		}
	}
	else if (MeshObject->GetMeshType() == Ideal::Skinned)
	{
		auto mesh = std::static_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject);
		RaytracingManagerDeleteObject(mesh);
		m_resourceManager->DeleteSkinnedMeshObject(mesh);
		auto it = std::find(m_skinnedMeshObject.begin(), m_skinnedMeshObject.end(), mesh);
		{
			if (it != m_skinnedMeshObject.end())
			{
				*it = std::move(m_skinnedMeshObject.back());
				m_deferredDeleteManager->AddMeshObjectToDeferredDelete(MeshObject);
				m_skinnedMeshObject.pop_back();
			}
		}
	}
}

void Ideal::D3D12RayTracingRenderer::DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> DebugMeshObject)
{
	// 아직 디버그 매쉬를 안만들고 static mesh에서 그냥 만드니 여기서 삭제
	auto mesh = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(DebugMeshObject);
	//RaytracingManagerDeleteObject(mesh);
	//
	//auto it = std::find(m_staticMeshObject.begin(), m_staticMeshObject.end(), mesh);
	//{
	//	*it = std::move(m_staticMeshObject.back());
	//	m_deferredDeleteManager->AddMeshObjectToDeferredDelete(DebugMeshObject);
	//	m_staticMeshObject.pop_back();
	//}
	m_deferredDeleteManager->AddMeshObjectToDeferredDelete(DebugMeshObject);
	m_debugMeshManager->DeleteDebugMesh(mesh);
}

std::shared_ptr<Ideal::IMeshObject> Ideal::D3D12RayTracingRenderer::CreateDebugMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealStaticMeshObject> newStaticMesh = std::make_shared<Ideal::IdealStaticMeshObject>();
	m_resourceManager->CreateStaticMeshObject(newStaticMesh, FileName);
	newStaticMesh->SetName(FileName);
	auto mesh = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(newStaticMesh);
	mesh->SetDebugMeshColor(Color(0, 1, 0, 1));

	if (m_isEditor)
	{
		m_debugMeshManager->AddDebugMesh(mesh);
	}
	return mesh;
}

std::shared_ptr<Ideal::IAnimation> Ideal::D3D12RayTracingRenderer::CreateAnimation(const std::wstring& FileName, const Matrix& offset /*= Matrix::Identity*/)
{
	std::shared_ptr<Ideal::IdealAnimation> newAnimation = std::make_shared<Ideal::IdealAnimation>();
	m_resourceManager->CreateAnimation(newAnimation, FileName, offset);

	return newAnimation;
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

void Ideal::D3D12RayTracingRenderer::ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData /*= false*/, bool NeedVertexInfo /*= false*/, bool NeedConvertCenter /*= false*/)
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

	assimpConverter->ExportModelData(FileName, isSkinnedData, NeedConvertCenter);
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

void Ideal::D3D12RayTracingRenderer::ConvertParticleMeshAssetToMyFormat(std::wstring FileName, bool SetScale /*= false*/, Vector3 Scale /*= Vector3(1.f)*/)
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

	assimpConverter->ExportParticleData(FileName, SetScale, Scale);
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
	if (m_isEditor)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();
	}
}

DirectX::SimpleMath::Vector2 Ideal::D3D12RayTracingRenderer::GetTopLeftEditorPos()
{
	//m_mainCameraEditorTopLeft;
	// 비율 계산
	auto& rect = m_postViewport->GetScissorRect();
	m_mainCameraEditorWindowSize;
	// offset * editor size / main window size = new Offset // return new Offset + editor pos
	float y = rect.top * m_mainCameraEditorWindowSize.y / m_height;
	float x = rect.left * m_mainCameraEditorWindowSize.x / m_width;

	y += m_mainCameraEditorTopLeft.y;
	x += m_mainCameraEditorTopLeft.x;
	return Vector2(x,y);
}

DirectX::SimpleMath::Vector2 Ideal::D3D12RayTracingRenderer::GetRightBottomEditorPos()
{
	//m_mainCameraEditorTopLeft;
	// 비율 계산
	auto& rect = m_postViewport->GetScissorRect();
	m_mainCameraEditorWindowSize;
	// offset * editor size / main window size = new Offset // return new Offset + editor pos
	float y = rect.top * m_mainCameraEditorWindowSize.y / m_height;
	float x = rect.left * m_mainCameraEditorWindowSize.x / m_width;

	y += m_mainCameraEditorBottomRight.y;
	x += m_mainCameraEditorBottomRight.x;
	return Vector2(x, y);
}

void Ideal::D3D12RayTracingRenderer::SetSkyBox(const std::wstring& FileName)
{
	std::shared_ptr <Ideal::D3D12Texture> skyBox = std::make_shared<Ideal::D3D12Texture>();
	m_resourceManager->CreateTextureDDS(skyBox, FileName);
	m_skyBoxTexture = skyBox;
}

std::shared_ptr<Ideal::ITexture> Ideal::D3D12RayTracingRenderer::CreateTexture(const std::wstring& FileName, bool IsGenerateMips /*= false*/, bool IsNormalMap /*= false*/)
{
	std::shared_ptr<Ideal::D3D12Texture> texture;
	uint32 generateMips = 1;
	if (IsGenerateMips)
	{
		generateMips = 0;
	}
	m_resourceManager->CreateTexture(texture, FileName,false, generateMips);

	//if (IsGenerateMips)
	//{
	//	m_resourceManager->GenerateMips(
	//		m_device,
	//		m_commandLists[m_currentContextIndex],
	//		m_descriptorHeaps[m_currentContextIndex],
	//		m_cbAllocator[m_currentContextIndex],
	//		texture,
	//		4	// TEMP
	//	);
	//}


	if (m_isEditor)
	{
		auto srv = texture->GetSRV();
		auto dest = m_imguiSRVHeap->Allocate();
		m_device->CopyDescriptorsSimple(1, dest.GetCpuHandle(), srv.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		texture->EmplaceSRVInEditor(dest);
	}
	return texture;
}

std::shared_ptr<Ideal::IMaterial> Ideal::D3D12RayTracingRenderer::CreateMaterial()
{
	std::shared_ptr<Ideal::IMaterial> ret = m_resourceManager->CreateMaterial();
	m_raytracingManager->CreateMaterialInRayTracing(m_device, m_descriptorManager, std::static_pointer_cast<Ideal::IdealMaterial>(ret));
	return ret;
}

void Ideal::D3D12RayTracingRenderer::DeleteTexture(std::shared_ptr<Ideal::ITexture> Texture)
{
	if (!Texture) return;
	m_deferredDeleteManager->AddTextureToDeferredDelete(std::static_pointer_cast<Ideal::D3D12Texture>(Texture));
	m_resourceManager->DeleteTexture(std::static_pointer_cast<Ideal::D3D12Texture>(Texture));
}

void Ideal::D3D12RayTracingRenderer::DeleteMaterial(std::shared_ptr<Ideal::IMaterial> Material)
{
	if (!Material) return;
	m_deferredDeleteManager->AddMaterialToDefferedDelete(std::static_pointer_cast<Ideal::IdealMaterial>(Material));
	m_raytracingManager->DeleteMaterialInRayTracing(Material);
}

std::shared_ptr<Ideal::ISprite> Ideal::D3D12RayTracingRenderer::CreateSprite()
{
	// TODO : Canvas에 UI를 추가해야 할 것이다.
	std::shared_ptr<Ideal::IdealSprite> ret = std::make_shared<Ideal::IdealSprite>();
	ret->SetScreenSize(Vector2(m_width, m_height));
	ret->SetMesh(m_resourceManager->GetDefaultQuadMesh());
	ret->SetTexture(m_resourceManager->GetDefaultAlbedoTexture());
	m_UICanvas->AddSprite(ret);
	return ret;
}

void Ideal::D3D12RayTracingRenderer::DeleteSprite(std::shared_ptr<Ideal::ISprite>& Sprite)
{
	auto s = std::static_pointer_cast<Ideal::IdealSprite>(Sprite);
	m_UICanvas->DeleteSprite(s);
	Sprite.reset();
}

std::shared_ptr<Ideal::IText> Ideal::D3D12RayTracingRenderer::CreateText(uint32 Width, uint32 Height, float FontSize, std::wstring Font /*= L"Tahoma"*/)
{
	auto fontHandle = m_textManager->CreateTextObject(Font.c_str(), FontSize);
	auto text = std::make_shared<Ideal::IdealText>(m_textManager, fontHandle);
	std::shared_ptr<Ideal::D3D12Texture> dynamicTexture;
	m_resourceManager->CreateDynamicTexture(dynamicTexture, Width, Height);
	{
		//Sprite
		std::shared_ptr<Ideal::IdealSprite> sprite = std::make_shared<Ideal::IdealSprite>();
		sprite->SetMesh(m_resourceManager->GetDefaultQuadMesh());
		sprite->SetTexture(m_resourceManager->GetDefaultAlbedoTexture());
		text->SetSprite(sprite);
	}
	text->SetTexture(dynamicTexture);
	text->ChangeText(L"Text : 텍스트");
	text->UpdateDynamicTextureWithImage(m_device);
	m_UICanvas->AddText(text);
	return text;
}

void Ideal::D3D12RayTracingRenderer::DeleteText(std::shared_ptr<Ideal::IText>& Text)
{
	if (Text)
		m_UICanvas->DeleteText(std::static_pointer_cast<Ideal::IdealText>(Text));
}

void Ideal::D3D12RayTracingRenderer::CompileShader(const std::wstring& FilePath, const std::wstring& SavePath, const std::wstring& SaveName, const std::wstring& ShaderVersion, const std::wstring& EntryPoint /*= L"Main"*/, const std::wstring& IncludeFilePath /*= L""*/)
{
	ComPtr<IDxcBlob> blob;
	// TODO : 쉐이더를 컴파일 해야 한다.
	std::shared_ptr<Ideal::ShaderManager> shaderManager = std::make_shared<Ideal::ShaderManager>();
	shaderManager->Init();
	shaderManager->AddIncludeDirectories(IncludeFilePath);
	shaderManager->CompileShaderAndSave(
		FilePath.c_str(),
		SavePath,
		SaveName.c_str(),
		ShaderVersion,
		blob,
		EntryPoint, //entry
		true
	);
}

std::shared_ptr<Ideal::IShader> Ideal::D3D12RayTracingRenderer::CreateAndLoadParticleShader(const std::wstring& Name)
{
	std::shared_ptr<Ideal::ShaderManager> shaderManager = std::make_shared<Ideal::ShaderManager>();
	std::shared_ptr<Ideal::D3D12Shader> shader = std::make_shared<Ideal::D3D12Shader>();
	shaderManager->Init();

	// TODO : 경로와 Name을 더해주어야 한다.
	std::wstring FilePath = L"../Shaders/Particle/" + Name + L".shader";

	shaderManager->LoadShaderFile(FilePath, shader);
	return std::static_pointer_cast<Ideal::IShader>(shader);
}

std::shared_ptr<Ideal::D3D12Shader> Ideal::D3D12RayTracingRenderer::CreateAndLoadShader(const std::wstring& FilePath)
{
	std::shared_ptr<Ideal::ShaderManager> shaderManager = std::make_shared<Ideal::ShaderManager>();
	std::shared_ptr<Ideal::D3D12Shader> shader = std::make_shared<Ideal::D3D12Shader>();
	shaderManager->Init();
	shaderManager->LoadShaderFile(FilePath, shader);
	return shader;
}

std::shared_ptr<Ideal::IParticleSystem> Ideal::D3D12RayTracingRenderer::CreateParticleSystem(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial)
{
	std::shared_ptr<Ideal::ParticleSystem> NewParticleSystem = std::make_shared<Ideal::ParticleSystem>();
	std::shared_ptr<Ideal::ParticleMaterial> GetParticleMaterial = std::static_pointer_cast<Ideal::ParticleMaterial>(ParticleMaterial);
	NewParticleSystem->Init(m_device, m_particleSystemManager->GetRootSignature(), m_particleSystemManager->GetVS(), GetParticleMaterial);
	if (GetParticleMaterial->GetTransparency())
	{
		m_particleSystemManager->AddParticleSystem(NewParticleSystem);
	}
	else
	{
		m_particleSystemManager->AddParticleSystemNoTransparency(NewParticleSystem);
	}
	return std::static_pointer_cast<Ideal::IParticleSystem>(NewParticleSystem);
}

void Ideal::D3D12RayTracingRenderer::DeleteParticleSystem(std::shared_ptr<Ideal::IParticleSystem>& ParticleSystem)
{
	m_particleSystemManager->DeleteParticleSystem(std::static_pointer_cast<Ideal::ParticleSystem>(ParticleSystem));
}

std::shared_ptr<Ideal::IParticleMaterial> Ideal::D3D12RayTracingRenderer::CreateParticleMaterial()
{
	std::shared_ptr<Ideal::ParticleMaterial> NewParticleMaterial = std::make_shared<Ideal::ParticleMaterial>();
	return std::static_pointer_cast<Ideal::IParticleMaterial>(NewParticleMaterial);
}

void Ideal::D3D12RayTracingRenderer::DeleteParticleMaterial(std::shared_ptr<Ideal::IParticleMaterial>& ParticleMaterial)
{

}

std::shared_ptr<Ideal::IMesh> Ideal::D3D12RayTracingRenderer::CreateParticleMesh(const std::wstring& FileName)
{
	auto mesh = m_resourceManager->CreateParticleMesh(FileName);
	return mesh;
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


	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
	//fullScreenDesc.RefreshRate.Numerator = 60;
	//fullScreenDesc.RefreshRate.Denominator = 1;
	//fullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//fullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//fullScreenDesc.Windowed = true;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Stereo = FALSE;
	//swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapChainDesc.Flags = m_tearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	m_swapChainFlags = swapChainDesc.Flags;

	ComPtr<IDXGISwapChain1> swapChain;
	Check(Factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		m_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain.GetAddressOf()
	));

	if (m_tearingSupport)
	{
		Check(Factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER), L"Failed to make window association");
	}
	//Check(Factory->MakeWindowAssociation(m_hwnd, NULL), L"Failed to make window association");
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
			m_renderTargets[i]->Create(resource, m_deferredDeleteManager);
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
	if (m_depthStencil)
	{
		m_depthStencil->Release();
	}
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
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
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
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

void Ideal::D3D12RayTracingRenderer::UpdatePostViewAndScissor(uint32 Width, uint32 Height)
{
	//float viewWidthRatio = static_cast<float>(Width) / m_width;
	//float viewHeightRatio = static_cast<float>(Height) / m_height;
	//
	//float x = 1.0f;
	//float y = 1.0f;
	//
	//if (viewWidthRatio < viewHeightRatio)
	//{
	//	// The scaled image's height will fit to the viewport's height and 
	//	// its width will be smaller than the viewport's width.
	//	x = viewWidthRatio / viewHeightRatio;
	//}
	//else
	//{
	//	// The scaled image's width will fit to the viewport's width and 
	//	// its height may be smaller than the viewport's height.
	//	y = viewHeightRatio / viewWidthRatio;
	//}
	//
	//m_viewport->ReSize()
	//
	//
	//m_postViewport.TopLeftX = m_width * (1.0f - x) / 2.0f;
	//m_postViewport.TopLeftY = m_height * (1.0f - y) / 2.0f;
	//m_postViewport.Width = x * m_width;
	//m_postViewport.Height = y * m_height;
	//
	//m_postScissorRect.left = static_cast<LONG>(m_postViewport.TopLeftX);
	//m_postScissorRect.right = static_cast<LONG>(m_postViewport.TopLeftX + m_postViewport.Width);
	//m_postScissorRect.top = static_cast<LONG>(m_postViewport.TopLeftY);
	//m_postScissorRect.bottom = static_cast<LONG>(m_postViewport.TopLeftY + m_postViewport.Height);
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

	//commandList->OMSetRenderTargets(1, &rtvHandle.GetCpuHandle(), FALSE, &dsvHandle);
	commandList->OMSetRenderTargets(1, &rtvHandle.GetCpuHandle(), FALSE, nullptr);
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
	PresentFlags = (m_tearingSupport && m_windowMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
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
	// ui
	m_mainDescriptorHeaps[nextContextIndex]->Reset();

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
}

void Ideal::D3D12RayTracingRenderer::CreatePostScreenRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE1 ranges[Ideal::PostScreenRootSignature::Slot::Count];
	CD3DX12_ROOT_PARAMETER1 rootParameters[Ideal::PostScreenRootSignature::Slot::Count];

	ranges[Ideal::PostScreenRootSignature::Slot::SRV_Scene].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	rootParameters[Ideal::PostScreenRootSignature::Slot::SRV_Scene].InitAsDescriptorTable(1, &ranges[Ideal::PostScreenRootSignature::Slot::SRV_Scene], D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_SIGNATURE_FLAGS rootSigantureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// Create a sampler.
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSigantureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	if (error)
	{
		MyMessageBox((char*)error->GetBufferPointer());
	}
	Check(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_postScreenRootSignature)));
}

void Ideal::D3D12RayTracingRenderer::CreatePostScreenPipelineState()
{
	// TEMP : shader compile
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> postVertexShader;
	ComPtr<ID3DBlob> postPixelShader;
	Check(D3DCompileFromFile(L"../Shaders/Screen/Screen.hlsl", nullptr, nullptr, "VS", "vs_5_0", compileFlags, 0, &postVertexShader, &error));
	if (error)
	{
		MyMessageBox((char*)error->GetBufferPointer());
	}
	Check(D3DCompileFromFile(L"../Shaders/Screen/Screen.hlsl", nullptr, nullptr, "PS", "ps_5_0", compileFlags, 0, &postPixelShader, &error));
	if (error)
	{
		MyMessageBox((char*)error->GetBufferPointer());
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { SimpleVertex::InputElements, SimpleVertex::InputElementCount };
	psoDesc.pRootSignature = m_postScreenRootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(postVertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(postPixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	Check(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_postScreenPipelineState)));
}

void Ideal::D3D12RayTracingRenderer::DrawPostScreen()
{
	std::shared_ptr<Ideal::D3D12Texture> renderTarget = m_renderTargets[m_frameIndex];
	m_commandLists[m_currentContextIndex]->RSSetViewports(1, &m_postViewport->GetViewport());
	m_commandLists[m_currentContextIndex]->RSSetScissorRects(1, &m_postViewport->GetScissorRect());
	m_commandLists[m_currentContextIndex]->OMSetRenderTargets(1, &renderTarget->GetRTV().GetCpuHandle(), FALSE, nullptr);

	m_commandLists[m_currentContextIndex]->SetPipelineState(m_postScreenPipelineState.Get());
	m_commandLists[m_currentContextIndex]->SetGraphicsRootSignature(m_postScreenRootSignature.Get());
	m_commandLists[m_currentContextIndex]->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto defaultQuadMesh = m_resourceManager->GetDefaultQuadMesh2();

	m_commandLists[m_currentContextIndex]->IASetVertexBuffers(0, 1, &defaultQuadMesh->GetVertexBufferView());
	m_commandLists[m_currentContextIndex]->IASetIndexBuffer(&defaultQuadMesh->GetIndexBufferView());
	auto handle = m_mainDescriptorHeaps[m_currentContextIndex]->Allocate();
	m_device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), m_raytracingManager->GetRaytracingOutputSRVHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_commandLists[m_currentContextIndex]->SetGraphicsRootDescriptorTable(Ideal::PostScreenRootSignature::Slot::SRV_Scene, handle.GetGpuHandle());
	m_commandLists[m_currentContextIndex]->DrawIndexedInstanced(defaultQuadMesh->GetElementCount(), 1, 0, 0, 0);
	//m_commandLists[m_currentContextIndex]->DrawInstanced(4, 1, 0, 0);

	auto raytracingOutput = m_raytracingManager->GetRaytracingOutputResource();

	CD3DX12_RESOURCE_BARRIER postCopyBarriers = CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &postCopyBarriers);
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
	//compiler->Compile(&sourceBuffer, nullptr, 0, includeHandler.Get()e, IID_PPV_ARGS(&result));
	compiler->Compile(&sourceBuffer, args, _countof(args), includeHandler.Get(), IID_PPV_ARGS(&result));


	ComPtr<IDxcBlobEncoding> encoding = nullptr;
	result->GetErrorBuffer(&encoding);
	if (encoding)
	{
		auto a = (char*)encoding->GetBufferPointer();
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

void Ideal::D3D12RayTracingRenderer::TransitionRayTracingOutputToRTV()
{
	ComPtr<ID3D12Resource> raytracingOutput = m_raytracingManager->GetRaytracingOutputResource();
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &barrier);

}

void Ideal::D3D12RayTracingRenderer::TransitionRayTracingOutputToSRV()
{
	ComPtr<ID3D12Resource> raytracingOutput = m_raytracingManager->GetRaytracingOutputResource();
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &barrier);

}

void Ideal::D3D12RayTracingRenderer::TransitionRayTracingOutputToUAV()
{
	return;
	ComPtr<ID3D12Resource> raytracingOutput = m_raytracingManager->GetRaytracingOutputResource();
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &barrier);
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
	m_raytracingManager->UpdateMaterial(m_device, m_deferredDeleteManager);
	m_raytracingManager->UpdateTexture(m_device);
	m_raytracingManager->UpdateAccelerationStructures(shared_from_this(), m_device, m_commandLists[m_currentContextIndex], m_BLASInstancePool[m_currentContextIndex], m_deferredDeleteManager);
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
		m_raytracingManager->BuildShaderTables(m_device, m_deferredDeleteManager);
	}

	auto instanceDesc = m_raytracingManager->AllocateInstanceByBLAS(blas);
	obj->SetBLASInstanceDesc(instanceDesc);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerAddBakedObject(std::shared_ptr<Ideal::IdealStaticMeshObject> obj)
{
	// 기존과 차이점은 이름으로 부르지 않는다.
	//auto blas = m_raytracingManager->GetBLASByName(obj->GetName().c_str());
	std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas;
	bool ShouldBuildShaderTable = true;

	// 안에서 add ref count를 실행시키긴 함. ....
	blas = m_raytracingManager->AddBLAS(shared_from_this(), m_device, m_resourceManager, m_descriptorManager, m_cbAllocator[m_currentContextIndex], obj, obj->GetName().c_str(), false);

	if (ShouldBuildShaderTable)
	{
		m_raytracingManager->BuildShaderTables(m_device, m_deferredDeleteManager);
	}

	auto instanceDesc = m_raytracingManager->AllocateInstanceByBLAS(blas);
	obj->SetBLASInstanceDesc(instanceDesc);
}

void Ideal::D3D12RayTracingRenderer::RaytracingManagerAddObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> obj)
{
	//ResetCommandList();
	auto blas = m_raytracingManager->AddBLAS(shared_from_this(), m_device, m_resourceManager, m_descriptorManager, m_cbAllocator[m_currentContextIndex], obj, obj->GetName().c_str(), true);
	// Skinning 데이터는 쉐이더 테이블을 그냥 만든다.
	m_raytracingManager->BuildShaderTables(m_device, m_deferredDeleteManager);

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
			m_deferredDeleteManager->AddBLASToDeferredDelete(blas);
			m_raytracingManager->DeleteBLAS(blas, name, false);
			m_raytracingManager->BuildShaderTables(m_device, m_deferredDeleteManager);
			// HitContributionToIndex 이거 갱신해줘야 한다.

		}
	}
	//obj.reset();

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
			m_deferredDeleteManager->AddBLASToDeferredDelete(blas);
			m_raytracingManager->DeleteBLAS(blas, name, true);
			m_raytracingManager->BuildShaderTables(m_device, m_deferredDeleteManager);

			//obj->GetBLASInstanceDesc()->InstanceDesc.InstanceContributionToHitGroupIndex = obj->GetBLAS()->GetInstanceContributionToHitGroupIndex();
		}
	}
	//obj.reset();
}

void Ideal::D3D12RayTracingRenderer::CreateUIDescriptorHeap()
{
	//------UI Descriptor Heap------//
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		m_mainDescriptorHeaps[i] = std::make_shared<Ideal::D3D12DescriptorHeap>();
		m_mainDescriptorHeaps[i]->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_DESCRIPTOR_COUNT);
	}
}

void Ideal::D3D12RayTracingRenderer::CreateCanvas()
{
	m_UICanvas = std::make_shared<Ideal::IdealCanvas>();
	m_UICanvas->Init(m_device);
	m_UICanvas->SetCanvasSize(m_width, m_height);
}

void Ideal::D3D12RayTracingRenderer::DrawCanvas()
{
	ID3D12DescriptorHeap* descriptorHeap[] = { m_mainDescriptorHeaps[m_currentContextIndex]->GetDescriptorHeap().Get() };
	m_commandLists[m_currentContextIndex]->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

	std::shared_ptr<Ideal::D3D12Texture> renderTarget = m_renderTargets[m_frameIndex];

	m_commandLists[m_currentContextIndex]->RSSetViewports(1, &m_viewport->GetViewport());
	m_commandLists[m_currentContextIndex]->RSSetScissorRects(1, &m_viewport->GetScissorRect());
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
#ifdef BeforeRefactor
	m_commandLists[m_currentContextIndex]->OMSetRenderTargets(1, &renderTarget->GetRTV().GetCpuHandle(), FALSE, &dsvHandle);
#else
	m_commandLists[m_currentContextIndex]->OMSetRenderTargets(1, &m_raytracingManager->GetRaytracingOutputRTVHandle().GetCpuHandle(), FALSE, &dsvHandle);
#endif
	m_UICanvas->DrawCanvas(m_device, m_commandLists[m_currentContextIndex], m_mainDescriptorHeaps[m_currentContextIndex], m_cbAllocator[m_currentContextIndex]);
}

void Ideal::D3D12RayTracingRenderer::SetCanvasSize(uint32 Width, uint32 Height)
{
	m_UICanvas->SetCanvasSize(Width, Height);
}

void Ideal::D3D12RayTracingRenderer::UpdateTextureWithImage(std::shared_ptr<Ideal::D3D12Texture> Texture, BYTE* SrcBits, uint32 SrcWidth, uint32 SrcHeight)
{
	if (SrcWidth > Texture->GetWidth())
	{
		__debugbreak();
	}
	if (SrcHeight > Texture->GetHeight())
	{
		__debugbreak();
	}
	D3D12_RESOURCE_DESC desc = Texture->GetResource()->GetDesc();
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
	uint32 rows = 0;
	uint64 rowSize = 0;
	uint64 TotalBytes = 0;
	m_device->GetCopyableFootprints(&desc, 0, 1, 0, &Footprint, &rows, &rowSize, &TotalBytes);

	BYTE* mappedPtr = nullptr;
	CD3DX12_RANGE writeRange(0, 0);
	HRESULT hr = Texture->GetUploadBuffer()->Map(0, &writeRange, reinterpret_cast<void**>(&mappedPtr));
	Check(hr);

	BYTE* pSrc = SrcBits;
	BYTE* pDest = mappedPtr;
	for (uint32 y = 0; y < SrcHeight; ++y)
	{
		memcpy(pDest, pSrc, SrcWidth * 4);
		pSrc += (SrcWidth * 4);
		pDest += Footprint.Footprint.RowPitch;
	}
	Texture->GetUploadBuffer()->Unmap(0, nullptr);
	Texture->SetUpdated();
}

void Ideal::D3D12RayTracingRenderer::CreateParticleSystemManager()
{
	m_particleSystemManager = std::make_shared<Ideal::ParticleSystemManager>();

	//---VertexShader---//
	// TEMP: 일단 그냥 여기서 컴파일 한다.
	//CompileShader(L"../Shaders/Particle/DefaultParticleVS.hlsl", L"DefaultParticleVS", L"vs_6_3", L"Main", L"../Shaders/Particle/");
	//CompileShader(L"../Shaders/Particle/DefaultParticleVS.hlsl", L"DefaultParticleVS", L"vs_6_3", L"Main", L"../Shaders/Particle/");
	CompileShader(L"../Shaders/Particle/DefaultParticleVS.hlsl", L"../Shaders/Particle/", L"DefaultParticleVS", L"vs_6_3", L"Main", L"../Shaders/Particle/");
	auto shader = CreateAndLoadParticleShader(L"DefaultParticleVS");

	//---Init---//
	m_particleSystemManager->Init(m_device, std::static_pointer_cast<Ideal::D3D12Shader>(shader));
}

void Ideal::D3D12RayTracingRenderer::DrawParticle()
{
	ID3D12DescriptorHeap* descriptorHeap[] = { m_mainDescriptorHeaps[m_currentContextIndex]->GetDescriptorHeap().Get() };
	m_commandLists[m_currentContextIndex]->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);

	std::shared_ptr<Ideal::D3D12Texture> renderTarget = m_renderTargets[m_frameIndex];

	m_commandLists[m_currentContextIndex]->RSSetViewports(1, &m_viewport->GetViewport());
	m_commandLists[m_currentContextIndex]->RSSetScissorRects(1, &m_viewport->GetScissorRect());
	//CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetGPUDescriptorHandleForHeapStart());
	// TODO : DSV SET
	auto depthBuffer = m_raytracingManager->GetDepthBuffer();
	//m_commandLists[m_currentContextIndex]->OMSetRenderTargets()
	//m_commandLists[m_currentContextIndex]->OMSetRenderTargets(1, &m_raytracingManager->GetRaytracingOutputRTVHandle().GetCpuHandle(), FALSE, &depthBuffer->GetDSV().GetCpuHandle());
	m_commandLists[m_currentContextIndex]->OMSetRenderTargets(1, &m_raytracingManager->GetRaytracingOutputRTVHandle().GetCpuHandle(), FALSE, &depthBuffer->GetDSV().GetCpuHandle());
	//m_commandLists[m_currentContextIndex]->OMSetRenderTargets(1, &m_raytracingManager->GetRaytracingOutputRTVHandle().GetCpuHandle(), FALSE, NULL);
	m_particleSystemManager->DrawParticles(m_device, m_commandLists[m_currentContextIndex], m_mainDescriptorHeaps[m_currentContextIndex], m_cbAllocator[m_currentContextIndex], &m_globalCB);
}

void Ideal::D3D12RayTracingRenderer::CreateDebugMeshManager()
{
	m_debugMeshManager = std::make_shared<Ideal::DebugMeshManager>();

	CompileShader(L"../Shaders/DebugMesh/DebugMeshShader.hlsl", L"../Shaders/DebugMesh/", L"DebugMeshShaderVS", L"vs_6_3", L"VSMain");
	CompileShader(L"../Shaders/DebugMesh/DebugMeshShader.hlsl", L"../Shaders/DebugMesh/", L"DebugMeshShaderPS", L"ps_6_3", L"PSMain");

	auto vs = CreateAndLoadShader(L"../Shaders/DebugMesh/DebugMeshShaderVS.shader");
	auto ps = CreateAndLoadShader(L"../Shaders/DebugMesh/DebugMeshShaderPS.shader");

	m_debugMeshManager->SetVS(vs);
	m_debugMeshManager->SetPS(ps);

	CompileShader(L"../Shaders/DebugMesh/DebugLineShader.hlsl", L"../Shaders/DebugMesh/", L"DebugLineShaderVS", L"vs_6_3", L"VSMain");
	CompileShader(L"../Shaders/DebugMesh/DebugLineShader.hlsl", L"../Shaders/DebugMesh/", L"DebugLineShaderPS", L"ps_6_3", L"PSMain");

	auto vsLine = CreateAndLoadShader(L"../Shaders/DebugMesh/DebugLineShaderVS.shader");
	auto psLine = CreateAndLoadShader(L"../Shaders/DebugMesh/DebugLineShaderPS.shader");

	m_debugMeshManager->SetVSLine(vsLine);
	m_debugMeshManager->SetPSLine(psLine);
	m_debugMeshManager->SetDebugLineVB(m_resourceManager->GetDebugLineVB());

	m_debugMeshManager->Init(m_device);
}

void Ideal::D3D12RayTracingRenderer::DrawDebugMeshes()
{
	if (m_isEditor)
	{
		m_debugMeshManager->DrawDebugMeshes(m_device, m_commandLists[m_currentContextIndex], m_mainDescriptorHeaps[m_currentContextIndex], m_cbAllocator[m_currentContextIndex], &m_globalCB);
	}
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
	ImGui::Begin("MAIN SCREEN", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);		// Create a window called "Hello, world!" and append into it.

	ImVec2 windowPos = ImGui::GetWindowPos();
 	ImVec2 min = ImGui::GetWindowContentRegionMin();
 	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 size(windowSize.x, windowSize.y);

	m_mainCameraEditorWindowSize.x = windowSize.x;
	m_mainCameraEditorWindowSize.y = windowSize.y;

	m_mainCameraEditorTopLeft.x = windowPos.x + min.x;
	m_mainCameraEditorTopLeft.y = windowPos.y + min.y;

	m_mainCameraEditorBottomRight.x = windowPos.x + max.x;
	m_mainCameraEditorBottomRight.y = windowPos.y + max.y;

	float viewWidthRatio = static_cast<float>(m_width) / windowSize.x;
	float viewHeightRatio = static_cast<float>(m_height) / windowSize.y;

	float x = 1.0f;
	float y = 1.0f;

	if (viewWidthRatio < viewHeightRatio)
	{
		// The scaled image's height will fit to the viewport's height and 
		// its width will be smaller than the viewport's width.
		x = viewWidthRatio / viewHeightRatio;
	}
	else
	{
		// The scaled image's width will fit to the viewport's width and 
		// its height may be smaller than the viewport's height.
		y = viewHeightRatio / viewWidthRatio;
	}
	size.x = x * windowSize.x;
	size.y = y * windowSize.y;

	ImGui::Image((ImTextureID)(m_editorTexture->GetSRV().GetGpuHandle().ptr), size);
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
	//commandList->RSSetViewports(1, &m_postViewport->GetViewport());
	//commandList->RSSetScissorRects(1, &m_viewport->GetScissorRect());
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
			m_editorTexture->Create(resource, m_deferredDeleteManager);

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

void Ideal::D3D12RayTracingRenderer::BakeOption(int32 MaxBakeCount, float MinSpaceSize)
{
	m_maxBakeCount = MaxBakeCount;
	m_octreeMinSpaceSize = MinSpaceSize;
}

void Ideal::D3D12RayTracingRenderer::BakeStaticMeshObject()
{
//#define OctreePosition
#define OctreeAABB
	m_Octree = Octree<std::shared_ptr<Ideal::IdealStaticMeshObject>>();
	for (auto& object : m_staticMeshObject)
	{
		if (object->GetIsStaticWhenRunTime())
		{
#ifdef OctreePosition
			Vector3 position;
			position.x = object->GetTransformMatrix()._41;
			position.y = object->GetTransformMatrix()._42;
			position.z = object->GetTransformMatrix()._43;
			m_Octree.AddObject(object, position);
#endif
#ifdef OctreeAABB
			//Vector3 position;
			//position.x = object->GetTransformMatrix()._41;
			//position.y = object->GetTransformMatrix()._42;
			//position.z = object->GetTransformMatrix()._43;
			
			Bounds bounds;
			Vector3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
			Vector3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			bounds.SetMinMax(minBounds, maxBounds);
			auto mesh = object->GetStaticMesh();
			auto submeshes = mesh->GetMeshes();
			for (auto m : submeshes)
			{
				Vector3 min = m->GetMinBound();
				Vector3 max = m->GetMaxBound();

				// 바운드의 최소 및 최대 값을 업데이트합니다.
				minBounds = Vector3::Min(minBounds, min);
				maxBounds = Vector3::Max(maxBounds, max);

				// 8개의 코너 포인트를 계산합니다.
				std::vector<Vector3> corners = {
					Vector3(min.x, min.y, min.z),
					Vector3(min.x, min.y, max.z),
					Vector3(min.x, max.y, min.z),
					Vector3(min.x, max.y, max.z),
					Vector3(max.x, min.y, min.z),
					Vector3(max.x, min.y, max.z),
					Vector3(max.x, max.y, min.z),
					Vector3(max.x, max.y, max.z)
				};

				// 각 코너 포인트를 변환하여 바운딩 박스에 캡슐화합니다.
				for (auto& corner : corners)
				{
					corner = Vector3::Transform(corner, object->GetTransformMatrix());
					bounds.Encapsulate(corner);
				}
			}

			// 최소 및 최대 바운드 변환
			Vector3 transformedMin = Vector3::Transform(minBounds, object->GetTransformMatrix());
			Vector3 transformedMax = Vector3::Transform(maxBounds, object->GetTransformMatrix());

			// 바운딩 박스에 최소 및 최대 값을 캡슐화합니다.
			bounds.Encapsulate(transformedMin);
			bounds.Encapsulate(transformedMax);

			// 바운딩 박스의 엣지를 추가합니다.
			auto edges = bounds.GetEdges();
			// for (const auto& e : edges)
			// {
			// 	m_debugMeshManager->AddDebugLine(e.first, e.second);
			// }

			m_Octree.AddObject(object, bounds);
#endif
		}
	}
#ifdef OctreePosition
	m_Octree.Bake(m_octreeMinSpaceSize);
#endif

#ifdef OctreeAABB
	m_Octree.BakeBoundVer(m_octreeMinSpaceSize);
#endif

	//for (auto& object : m_staticMeshObject)
	//{
	//	if (object->GetIsStaticWhenRunTime())
	//	{
	//		Bounds bounds;
	//		auto mesh = object->GetStaticMesh();
	//		auto submeshes = mesh->GetMeshes();
	//		for (auto m : submeshes)
	//		{
	//			Vector3 min = m->GetMinBound();
	//			Vector3 max = m->GetMaxBound();
	//			min = Vector3::Transform(min, object->GetTransformMatrix());
	//			max = Vector3::Transform(max, object->GetTransformMatrix());
	//			bounds.SetMinMax(min, max);
	//		}
	//	}
	//}

#ifdef _DEBUG
	{
		//// 라인 그리기
		//std::vector<std::pair<Vector3, Vector3>> lines;
		//m_Octree.ForeachNodeInternal(
		//	[&](std::shared_ptr<OctreeNode<std::shared_ptr<Ideal::IdealStaticMeshObject>>> Node)
		//	{
		//		Bounds bound = Node->GetBounds();
		//		auto edges = bound.GetEdges();
		//		for (auto& e : edges)
		//		{
		//			m_debugMeshManager->AddDebugLine(e.first, e.second);
		//		}
		//	}
		//);

		//int a = 3;
	}
#endif
}

void Ideal::D3D12RayTracingRenderer::ReBuildBLASFlagOn()
{
	m_ReBuildBLASFlag = true;
}

void Ideal::D3D12RayTracingRenderer::ReBuildBLAS()
{
	//return;

	m_ReBuildBLASFlag = false;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//static int once = 0;
	//if (once > 0) return;
	//once++;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TODO : 기존 baked object 삭제?
	//struct Obj
	//{
	//	std::shared_ptr<Ideal::IdealStaticMeshObject> staticMesh;
	//	Bounds bounds;
	//};
	//std::vector<Obj> boundsVector;
	//for (auto& object : m_staticMeshObject)
	//{
	//	if (object->GetIsStaticWhenRunTime())
	//	{
	//		Bounds bounds;
	//		Vector3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
	//		Vector3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//		bounds.SetMinMax(minBounds, maxBounds);
	//		auto mesh = object->GetStaticMesh();
	//		auto submeshes = mesh->GetMeshes();
	//		for (auto m : submeshes)
	//		{
	//			Vector3 min = m->GetMinBound();
	//			Vector3 max = m->GetMaxBound();
	//			min = Vector3::Transform(min, object->GetTransformMatrix());
	//			max = Vector3::Transform(max, object->GetTransformMatrix());
	//			bounds.Encapsulate(min);
	//			bounds.Encapsulate(max);
	//		}
	//		Obj obj;
	//		obj.staticMesh = object;
	//		obj.bounds = bounds;
	//		boundsVector.push_back(obj);
	//	}
	//}
	//std::vector<std::vector<Obj>> mergedGroups; // 병합된 그룹 리스트
	//std::vector<bool> visited(boundsVector.size(), false);
	//
	//std::shared_ptr<Ideal::IdealStaticMeshObject> current;
	//
	//for (size_t i = 0; i < boundsVector.size(); ++i) {
	//	if (visited[i]) continue;
	//
	//	std::vector<Obj> group;
	//	group.push_back(boundsVector[i]);
	//	visited[i] = true;
	//
	//	for (size_t j = i + 1; j < boundsVector.size(); ++j) {
	//		if (visited[j]) continue;
	//
	//		// 겹치는지 확인
	//		if (boundsVector[i].bounds.Intersects(boundsVector[j].bounds)) {
	//			group.push_back(boundsVector[j]);
	//			visited[j] = true;
	//		}
	//	}
	//
	//	mergedGroups.push_back(group);
	//}

	int b = 3;

	std::vector<std::shared_ptr<OctreeNode<std::shared_ptr<Ideal::IdealStaticMeshObject>>>> nodes;
	m_Octree.GetFinalNodes(nodes);

	for (auto& node : nodes)
	//for (auto& groups : mergedGroups)
	{
		uint32 BlasGeometryMaxSize = m_maxBakeCount;
		uint32 BlasGeometryCurrentSize = 0;
		auto& objects = node->GetObjects();
		//auto& objects = groups;

		// 09.25
		// 여기서 BLAS에 들어갈 Geometry들을 구해야 한다.
		// 이거 ComputeShader 사용해서 Vertex Buffer를 다시 구해야 할 것 같다.
		// 해야 하는 것이 결국 BLAS 하나를 만드는 것
		// 기존에 걸려있던 object의 BLAS를 일단 자르고 - 삭제하고
		// object들이 가지고 있는 Transform과 VertexBuffer를 곱해서 새로운 Vertex Buffer를 만들어서
		// Geometry정보에 넘겨준다면?
		//  (/^^)/  개쌉 계획은 완벽해보임	\(^^\)
		// Root Signature 만들고 PipelineState 만들고 Compute Shader 만들고 UAV 만들고...
		// Dispatch도 때려주고~ 졸라 계획 완벽하다.

		//09.26A
		/// tlqkf 진짜 졸라안쳐되네

		// 09.25 
		// 됐다. 이거 32개씩 나눠준다. 이제
		std::shared_ptr<Ideal::IdealStaticMeshObject> BakedMeshObject = std::make_shared<Ideal::IdealStaticMeshObject>();
		std::shared_ptr<Ideal::IdealStaticMesh> BakedStaticMesh = std::make_shared<Ideal::IdealStaticMesh>();
		BakedMeshObject->SetStaticMesh(BakedStaticMesh);
		for (auto& obj : objects)
		{
			//auto& obj = object.staticMesh;

			RaytracingManagerDeleteObject(obj);

			auto& meshes = obj->GetStaticMesh()->GetMeshes();
			CB_Transform transform;
			transform.World = obj->GetTransformMatrix().Transpose();
			transform.WorldInvTranspose = transform.World.Transpose().Invert();
			int meshSize = meshes.size();


			for (int i = 0; i < meshSize; ++i)
			{
				auto& mesh = meshes[i];
				std::shared_ptr<Ideal::D3D12UAVBuffer> newVertexBufferUAV = std::make_shared<Ideal::D3D12UAVBuffer>();
				std::wstring name = L"UAV_ModifiedVertexBuffer";
				uint32 vertexCount = mesh->GetElementCount();
				uint32 size = vertexCount * sizeof(BasicVertex);
				newVertexBufferUAV->Create(m_device.Get(), size, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, name.c_str());
				std::shared_ptr<Ideal::D3D12UnorderedAccessView> uav = m_resourceManager->CreateUAV(newVertexBufferUAV, vertexCount, sizeof(BasicVertex));
				newVertexBufferUAV->SetUAV(uav);
				DispatchModifyVertexBuffer(mesh, transform, newVertexBufferUAV);

				// NewMesh
				std::shared_ptr<Ideal::D3D12VertexBuffer> newVertexBuffer = std::make_shared<Ideal::D3D12VertexBuffer>();
				newVertexBuffer->CreateAndCopyResource
				(
					m_device,
					sizeof(BasicVertex),
					vertexCount,
					m_commandLists[m_currentContextIndex],
					newVertexBufferUAV,
					D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
				);

				m_tempUAVS.push_back(newVertexBufferUAV);
				uav->GetHandle().Free();
				//m_deferredDeleteManager->AddD3D12ResourceToDelete(newVertexBufferUAV->GetResourceComPtr());

				std::shared_ptr <Ideal::IdealMesh<BasicVertex>> newMesh = std::make_shared<Ideal::IdealMesh<BasicVertex>>();
				newMesh->SetName(mesh->GetName());
				// 기존은 기본 머테리얼이지만 다시
				// newMesh->SetMaterial(m_resourceManager->GetDefaultMaterial());
				// 
				newMesh->TransferMaterialInfo(mesh);

				newMesh->SetVertexBuffer(newVertexBuffer);
				newMesh->SetIndexBuffer(mesh->GetIndexBuffer());
				BakedStaticMesh->AddMesh(newMesh);


				BlasGeometryCurrentSize++;
				if (BlasGeometryCurrentSize >= BlasGeometryMaxSize)
				{
					BlasGeometryCurrentSize = 0;
					m_bakedMesh.push_back(BakedMeshObject);
					RaytracingManagerAddBakedObject(BakedMeshObject);

					BakedMeshObject = std::make_shared<Ideal::IdealStaticMeshObject>();
					BakedStaticMesh = std::make_shared<Ideal::IdealStaticMesh>();
					BakedMeshObject->SetStaticMesh(BakedStaticMesh);
				}
			}

			m_resourceManager->DeleteStaticMeshObject(obj);
			auto it = std::find(m_staticMeshObject.begin(), m_staticMeshObject.end(), obj);
			{
				if (it != m_staticMeshObject.end())
				{
					*it = std::move(m_staticMeshObject.back());
					m_deferredDeleteManager->AddMeshObjectToDeferredDelete(obj);
					m_staticMeshObject.pop_back();
				}
			}
		}
		m_bakedMesh.push_back(BakedMeshObject);
		RaytracingManagerAddBakedObject(BakedMeshObject);
	}
}

void Ideal::D3D12RayTracingRenderer::InitModifyVertexBufferShader()
{
	CompileShader(L"../Shaders/ModifyVertexBuffer/CS_ModifyVertexBuffer.hlsl", L"../Shaders/ModifyVertexBuffer/", L"ModifyVertexBufferCS", L"cs_6_3", L"CSMain");
	m_ModifyVertexBufferCS = CreateAndLoadShader(L"../Shaders/ModifyVertexBuffer/ModifyVertexBufferCS.shader");
}

void Ideal::D3D12RayTracingRenderer::CreateModifyVertexBufferRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::Count];
	ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::SRV_Vertices].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 : vertices
	ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_Transform].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0 : transform
	ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_VertexCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1); // b1 : vertex Count
	ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::UAV_OutputVertices].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0 : output vertices

	CD3DX12_ROOT_PARAMETER rootParameters[Ideal::ModifyVertexBufferCSRootSignature::Slot::Count];
	rootParameters[Ideal::ModifyVertexBufferCSRootSignature::Slot::SRV_Vertices].InitAsDescriptorTable(1, &ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::SRV_Vertices]);
	rootParameters[Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_Transform].InitAsDescriptorTable(1, &ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_Transform]);
	rootParameters[Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_VertexCount].InitAsDescriptorTable(1, &ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_VertexCount]);
	rootParameters[Ideal::ModifyVertexBufferCSRootSignature::Slot::UAV_OutputVertices].InitAsDescriptorTable(1, &ranges[Ideal::ModifyVertexBufferCSRootSignature::Slot::UAV_OutputVertices]);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		ARRAYSIZE(rootParameters), rootParameters, 0, nullptr
	);

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	if (error)
	{
		const wchar_t* msg = static_cast<wchar_t*>(error->GetBufferPointer());
		Check(hr, msg);
	}
	Check(m_device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_ModifyVertexBufferRootSignature)), L"Failed To Create Modify VB CS Rootsignature");

	m_ModifyVertexBufferRootSignature->SetName(L"ModifyVertexBufferRootSignature");
}

void Ideal::D3D12RayTracingRenderer::CreateModifyVertexBufferCSPipelineState()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
	computePipelineStateDesc.pRootSignature = m_ModifyVertexBufferRootSignature.Get();
	// TODO: Shader 만들어줘야한다.
	computePipelineStateDesc.CS = m_ModifyVertexBufferCS->GetShaderByteCode();

	Check(
		m_device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_ModifyVertexBufferPipelineState))
		, L"Failed To Create Modify Vertex Buffer Compute Pipeline State"
	);
}

void Ideal::D3D12RayTracingRenderer::DispatchModifyVertexBuffer(std::shared_ptr<Ideal::IdealMesh<BasicVertex>> Mesh, CB_Transform TransformData, std::shared_ptr<Ideal::D3D12UAVBuffer> UAVBuffer)
{
	//ComPtr<ID3D12CommandAllocator> CommandAllocator = m_commandAllocators[m_currentContextIndex];
	ComPtr<ID3D12GraphicsCommandList> CommandList = m_commandLists[m_currentContextIndex];
	//
	//Check(CommandAllocator->Reset(), L"Failed to reset commandAllocator!");
	//Check(CommandList->Reset(CommandAllocator.Get(), nullptr), L"Failed to reset commandList");

	std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap = m_mainDescriptorHeaps[m_currentContextIndex];
	std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool = m_cbAllocator[m_currentContextIndex];

	std::shared_ptr<Ideal::D3D12VertexBuffer> VertexBuffer = Mesh->GetVertexBuffer();

	CommandList->SetComputeRootSignature(m_ModifyVertexBufferRootSignature.Get());
	CommandList->SetPipelineState(m_ModifyVertexBufferPipelineState.Get());

	CommandList->SetDescriptorHeaps(1, DescriptorHeap->GetDescriptorHeap().GetAddressOf());

	// Parameter0 : SRV_Vertices
	auto handle0 = DescriptorHeap->Allocate();
	auto vertexSRV = m_resourceManager->CreateSRV(VertexBuffer, VertexBuffer->GetElementCount(), VertexBuffer->GetElementSize());
	m_device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), vertexSRV->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(Ideal::ModifyVertexBufferCSRootSignature::Slot::SRV_Vertices, handle0.GetGpuHandle());

	// Parameter1 : CBV_Transform
	auto handle1 = DescriptorHeap->Allocate();
	auto cb1 = CBPool->Allocate(m_device.Get(), sizeof(CB_Transform));
	memcpy(cb1->SystemMemAddr, &TransformData, sizeof(CB_Transform));
	m_device->CopyDescriptorsSimple(1, handle1.GetCpuHandle(), cb1->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_Transform, handle1.GetGpuHandle());

	// Parameter2 : CBV_VertexCount
	auto handle2 = DescriptorHeap->Allocate();
	auto cb2 = CBPool->Allocate(m_device.Get(), sizeof(uint32));
	uint32 vertexCount = VertexBuffer->GetElementCount();
	memcpy(cb2->SystemMemAddr, &vertexCount, sizeof(uint32));
	m_device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb2->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(Ideal::ModifyVertexBufferCSRootSignature::Slot::CBV_VertexCount, handle2.GetGpuHandle());

	// Parameter3: UAV_OutputVertices
	auto handle3 = DescriptorHeap->Allocate();
	auto uav = UAVBuffer->GetUAV();
	m_device->CopyDescriptorsSimple(1, handle3.GetCpuHandle(), uav->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(Ideal::ModifyVertexBufferCSRootSignature::Slot::UAV_OutputVertices, handle3.GetGpuHandle());

	// Barrier0
	CD3DX12_RESOURCE_BARRIER barrier0 = CD3DX12_RESOURCE_BARRIER::Transition(
		UAVBuffer->GetResource(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	CommandList->ResourceBarrier(1, &barrier0);

	// Dispatch
	uint32 elementCount = VertexBuffer->GetElementCount();
	uint32 threadsPreGroup = 1024;
	uint32 dispatchX = (elementCount + threadsPreGroup - 1) / threadsPreGroup;
	CommandList->Dispatch(dispatchX, 1, 1);

	// Barrier1
	CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::UAV(UAVBuffer->GetResource());
	CommandList->ResourceBarrier(1, &barrier1);

	// Barrier2
	CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
		UAVBuffer->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	CommandList->ResourceBarrier(1, &barrier2);
}

void Ideal::D3D12RayTracingRenderer::ReleaseBakedObject()
{
	//for (auto& uav : m_tempUAVS)
	//{
	//	uav->GetUAV()->GetHandle().Free();
	//}
	for (auto& mesh : m_bakedMesh)
	{
		RaytracingManagerDeleteObject(mesh);
	}
}
