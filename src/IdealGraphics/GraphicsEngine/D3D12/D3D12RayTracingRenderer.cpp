#include "D3D12RayTracingRenderer.h"

#include <DirectXColors.h>
#include "Misc/Utils/PIX.h"
#include "Misc/Assimp/AssimpConverter.h"

//#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
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

#include "GraphicsEngine/Resource/IdealCamera.h"
#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealAnimation.h"
#include "GraphicsEngine/Resource/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealRenderScene.h"
#include "GraphicsEngine/Resource/IdealScreenQuad.h"

#include "GraphicsEngine/Resource/Light/IdealDirectionalLight.h"
#include "GraphicsEngine/Resource/Light/IdealSpotLight.h"
#include "GraphicsEngine/Resource/Light/IdealPointLight.h"

#include "GraphicsEngine/D3D12/TestShader.h"

#include <d3dx12.h>
//#include "d3dx12.h"
#include <dxcapi.h>
#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

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
	m_raytacingOutputResourceUAVGpuDescriptorHandle.Free();

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

			if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1))))
			{
				debugController1->SetEnableGPUBasedValidation(true);
			}
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

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1 };
	DWORD featureLevelNum = _countof(featureLevels);
	DXGI_ADAPTER_DESC1 adpaterDesc = {};
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	for (uint32 featerLevelIndex = 0; featerLevelIndex < featureLevelNum; ++featerLevelIndex)
	{
		uint32 adapterIndex = 0;
		while (DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter))
		{
			adapter->GetDesc1(&adpaterDesc);

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), featureLevels[featerLevelIndex], IID_PPV_ARGS(m_device.GetAddressOf()))))
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
		FailedDebugBox(L"Device dose not support ray tracing!");
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

	CreateShaderCompiler();
	CreateDescriptorHeap();
	// --- Test --- //
	m_rayGenCB.viewport = { -1.f,-1.f,1.f,1.f };
	UpdateForSizeChange(m_width, m_height);
	CreateDeviceDependentResources();
}

void Ideal::D3D12RayTracingRenderer::Tick()
{
	// 테스트 용으로 쓰는 곳
	__debugbreak();
	return;
}

void Ideal::D3D12RayTracingRenderer::Render()
{
	ResetCommandList();
	m_mainCamera->UpdateMatrix2();

	BeginRender();
	DoRaytracing();
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
	return newCamera;
}

void Ideal::D3D12RayTracingRenderer::SetMainCamera(std::shared_ptr<ICamera> Camera)
{
	m_mainCamera = std::static_pointer_cast<Ideal::IdealCamera>(Camera);
}

std::shared_ptr<Ideal::IMeshObject> Ideal::D3D12RayTracingRenderer::CreateStaticMeshObject(const std::wstring& FileName)
{
	// 인터페이스로 따로 뽑아야 할 듯
	return nullptr;
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
	// 인터페이스로 따로 뽑아야 할 듯
	return nullptr;
}

void Ideal::D3D12RayTracingRenderer::SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene)
{
	// 인터페이스로 따로 뽑아야 할 듯
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
}

void Ideal::D3D12RayTracingRenderer::SetModelPath(const std::wstring& ModelPath)
{
}

void Ideal::D3D12RayTracingRenderer::SetTexturePath(const std::wstring& TexturePath)
{
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
	hr = m_swapChain->Present(1, 0);
	Check(hr);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	uint32 nextContextIndex = (m_currentContextIndex + 1) % MAX_PENDING_FRAME_COUNT;
	WaitForFenceValue(m_lastFenceValues[nextContextIndex]);

	m_descriptorHeaps[nextContextIndex]->Reset();
	m_cbAllocator[nextContextIndex]->Reset();

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

void Ideal::D3D12RayTracingRenderer::UploadBuffer(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info, ComPtr<ID3D12Resource> Scratch, ComPtr<ID3D12Resource> Result)
{
	// UAV 업로드
	//auto uavDesc =CD3DX12_RESOURCE_DESC::Buffer()
	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 0;
	bufferDesc.Width = info.ResultDataMaxSizeInBytes;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// allow
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);

	m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&blasResult)
	);

	bufferDesc.Width = info.ScratchDataSizeInBytes;

	m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&blasScratch)
	);
}

void Ideal::D3D12RayTracingRenderer::CreateAS(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS input, ComPtr<ID3D12Resource> Scratch, ComPtr<ID3D12Resource> Result)
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.Inputs = input;
	desc.ScratchAccelerationStructureData = blasScratch->GetGPUVirtualAddress();
	desc.DestAccelerationStructureData = blasResult->GetGPUVirtualAddress();

	// Create
	m_commandLists[m_currentContextIndex]->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

	// ResourceBarrier
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		blasResult.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	);
	m_commandLists[m_currentContextIndex]->ResourceBarrier(1, &barrier);
}

void Ideal::D3D12RayTracingRenderer::CreateDeviceDependentResources()
{
	CreateRootSignatures();
	//CreateRayTracingInterfaces();
	CreateRayTracingPipelineStateObject();
	BuildGeometry();
	BuildAccelerationStructures();
	BuildShaderTables();
	CreateRayTracingOutputResources();
}

void Ideal::D3D12RayTracingRenderer::CreateShaderCompiler()
{
	Check(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)), L"Failed To Create DXC Compiler Instance");
	Check(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_library)), L"Failed To Create DXC Library Instance");
}

void Ideal::D3D12RayTracingRenderer::CompileShader(IDxcLibrary* lib, IDxcCompiler* comp, LPCWSTR filename, IDxcBlob** blob)
{
	uint32 codePage = 0;
	IDxcBlobEncoding* pShaderText = nullptr;
	IDxcOperationResult* result = nullptr;

	// 셰이더 파일을 로드하고 인코딩한다.
	lib->CreateBlobFromFile(filename, &codePage, &pShaderText);

	// 셰이더 컴파일, "main"은 실행이 시작하는 곳
	comp->Compile(pShaderText, filename, L"main", L"lib_6_3", nullptr, 0, nullptr, 0, dxcIncIncludeHandler, &result);

	result->GetResult(blob);
}

void Ideal::D3D12RayTracingRenderer::InitializeTLAS()
{
	// TLAS Desc
	D3D12_RAYTRACING_INSTANCE_DESC instances = {};
	// 셰이더에서 사용 가능
	instances.InstanceID = 0;
	// 충돌 그룹 셰이더 선택
	instances.InstanceContributionToHitGroupIndex = 0;
	// TraceRay() 파라미터로 비트 단위 AND
	instances.InstanceMask = 1;
	instances.Transform[0][0] = instances.Transform[1][1] = instances.Transform[2][2] = 1;
	// 투명한가? 컬링인가?
	instances.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instances.AccelerationStructure = blasResult->GetGPUVirtualAddress();

	//D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS ASInputs = {};
	ASInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	ASInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	ASInputs.NumDescs = 1;
	ASInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO ASBuildInfo = {};
	m_device->GetRaytracingAccelerationStructurePrebuildInfo(&ASInputs, &ASBuildInfo);

	UploadBuffer(ASBuildInfo, tlasScratch, tlasResult);
	CreateAS(ASInputs, tlasScratch, tlasResult);
}

void Ideal::D3D12RayTracingRenderer::CreateRayTracingInterfaces()
{

}

void Ideal::D3D12RayTracingRenderer::CreateRootSignatures()
{
	// Global Root Signature
	// 이 루트 시그니쳐는 DispatchRays()가 호출되는 동안 모든 레이트레이싱에서의 접근이 허용된다.
	{
		CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
		UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
		rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &UAVDescriptor);
		rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		SerializeAndCreateRayTracingRootSignature(globalRootSignatureDesc, &m_raytracingGlobalRootSignature);
	}
	// Local Root Signature
	// 이 루트 시그니쳐는 쉐이더 테이블에서 나온 유니크 인자들을 쉐이더가 가질 수 있게 한다.
	{
		CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
		rootParameters[LocalRootSignatureParams::ViewportConstantSlot].InitAsConstants(SizeOfInUint32(m_rayGenCB), 0, 0);
		CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		SerializeAndCreateRayTracingRootSignature(localRootSignatureDesc, &m_raytracingLocalRootSignature);
	}
}

void Ideal::D3D12RayTracingRenderer::CreateRootSignatures2()
{
	// Global Root Signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
	{
		CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
		UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
		rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &UAVDescriptor);
		rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		SerializeAndCreateRayTracingRootSignature(globalRootSignatureDesc, &m_raytracingGlobalRootSignature);
	}

	// Local Root Signature
	// This is a root signature that enables a shader to have unique arguments that come from shader tables.
	{
		CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
		rootParameters[LocalRootSignatureParams::ViewportConstantSlot].InitAsConstants(SizeOfInUint32(m_rayGenCB), 0, 0);
		CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		SerializeAndCreateRayTracingRootSignature(localRootSignatureDesc, &m_raytracingLocalRootSignature);
	}
}

void Ideal::D3D12RayTracingRenderer::SerializeAndCreateRayTracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootsig)
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;

	Check(
		D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error),
		error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr
	);
	Check(
		m_device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootsig))),
		L"Failed to Create RootSignature!"
	);
}

void Ideal::D3D12RayTracingRenderer::CreateRayTracingPipelineStateObject()
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
	D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracing, ARRAYSIZE(g_pRaytracing));
	lib->SetDXILLibrary(&libdxil);
	// 라이브러리에서 표면화할 쉐이더 export를 정의한다
	// 만약 DXIL 라이브러리 서브 오브젝트에 대해 정의된 셰이더 익스포트가 없는 경우 모든 쉐이더가 표면화된다.
	// 이 샘플에서는 라이브러리의 모든 쉐이더를 사용하므로 편의상 생략할 수 있다.
	{
		lib->DefineExport(m_raygenShaderName);
		lib->DefineExport(m_closestHitShaderName);
		lib->DefineExport(m_missShaderName);
	}
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

	// Create StateObject
	Check(
		m_device->CreateStateObject(
			raytracingPipeline,
			IID_PPV_ARGS(&m_dxrStateObject)),
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
	rootSignatureAssociation->AddExport(m_raygenShaderName);

}

void Ideal::D3D12RayTracingRenderer::BuildGeometry()
{
	std::vector<uint32> indices = { 0,1,2 };

	float depthValue = 1.0;
	float offset = 0.7f;
	std::vector<GeometryVertex> vertices;
	{
		indices = { 0,1,2 };
		float depthValue = 1.0;
		float offset = 0.7f;
		vertices =
		{
			{Vector3(0, -offset, depthValue)},
			{Vector3(-offset, offset, depthValue)},
			{Vector3(offset, offset, depthValue)}
		};
	}

	AllocateUploadBuffer(m_device.Get(), vertices.data(), sizeof(GeometryVertex) * vertices.size(), &m_vb2);
	AllocateUploadBuffer(m_device.Get(), indices.data(), sizeof(uint32) * indices.size(), &m_ib2);

	//m_vertexBuffer = std::make_shared<Ideal::D3D12VertexBuffer>();
	//m_indexBuffer = std::make_shared<Ideal::D3D12IndexBuffer>();

	//m_resourceManager->CreateVertexBuffer(m_vertexBuffer, vertices);
	//m_resourceManager->CreateIndexBuffer(m_indexBuffer, indices);

}

void Ideal::D3D12RayTracingRenderer::BuildAccelerationStructures()
{

	ComPtr<ID3D12GraphicsCommandList4> commandList = m_commandLists[m_currentContextIndex];
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandAllocators[m_currentContextIndex];

	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.IndexBuffer = m_ib2->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexCount = static_cast<uint32>(m_ib2->GetDesc().Width) / sizeof(uint32);
	geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	geometryDesc.Triangles.Transform3x4 = 0;
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.VertexCount = static_cast<uint32>(m_vb2->GetDesc().Width) / sizeof(GeometryVertex);
	geometryDesc.Triangles.VertexBuffer.StartAddress = m_vb2->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(GeometryVertex);


	/*D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.IndexBuffer = m_indexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexCount = m_indexBuffer->GetElementCount();
	geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	geometryDesc.Triangles.Transform3x4 = 0;
	geometryDesc.Triangles.VertexFormat = GeometryVertex::VertexFormat;
	geometryDesc.Triangles.VertexCount = m_vertexBuffer->GetElementCount();
	geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = m_vertexBuffer->GetElementSize();*/

	// 퍼포먼스 팁: 지오메트리를 불투명으로 표시하면 중요한 광선 처리 최적화를 활성화할 수 있으므로 해당될 때마다 불투명으로 표시하세요.
	// 참고: 광선이 불투명한 지오메트리를 만나면 히트 셰이더는 존재 여부와 상관없이 실행되지 않습니다.
	// 불투명
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	// 가속구조에 필요한 사이즈를 가져온다
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	//---------TLAS----------//
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.Flags = buildFlags;
	topLevelInputs.NumDescs = 1;
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
	m_device->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
	Check(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	//---------BLAS---------//
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = topLevelInputs;
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs = &geometryDesc;
	m_device->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
	Check(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	ComPtr<ID3D12Resource> scratchResource;
	AllocateUAVBuffer(m_device.Get(), max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes), scratchResource.GetAddressOf(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

	// 가속 구조에 리소스를 할당
	// 가속 구조체는 기본 힙( 또는 이에 상응하는 사용자 정의 힙)에 생성된 리소스에만 배치할 수 있다.
	// 기본 힙은 애플리케이션에 CPU 읽기/쓰기 액세스가 필요하지 않으므로 괜찮다
	// 가속 구조가 포함될 리소스는 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE 상태로 생성되어야 한다.
	// 리소스 플래그 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS가 있어야 한다. ALLOW_UNORDERED_ACCESS 요구 사항은 다음의 두 가지를 모두 인정한다.
	//	- 시스템이 백그라운드에서 가속 구조 빌드를 구현할 때 이러한 유형의 액세스를 수행한다.
	//	- 앱의 관점에서 가속 구조에 대한 쓰기/읽기 동기화는 UAV 배리어를 사용하여 수행된다.
	{
		D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

		AllocateUAVBuffer(m_device.Get(), bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_bottomLevelAccelerationStructure, initialResourceState, L"BottomLevelAccelerationStructure");
		AllocateUAVBuffer(m_device.Get(), topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");
	}

	// 바텀 레벨 가속 구조에 필요한 instance desc을 만든다
	ComPtr<ID3D12Resource> instanceDescs;
	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
	instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1; // identity
	instanceDesc.InstanceMask = 1;
	instanceDesc.AccelerationStructure = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	AllocateUploadBuffer(m_device.Get(), &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");

	// 바텀 레벨 가속 구조 desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	{
		bottomLevelBuildDesc.Inputs = bottomLevelInputs;
		bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
		bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	}

	// 탑 레벨 가속 구조 desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	{
		topLevelInputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
		topLevelBuildDesc.Inputs = topLevelInputs;
		topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
		topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
	}

	// 가속 구조 빌드
	commandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure.Get());
		commandList->ResourceBarrier(1, &barrier);
	}
	commandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);

	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Fence();
	WaitForFenceValue(m_lastFenceValues[m_currentContextIndex]);

	/*Ideal::D3D12UploadBuffer tempUploadBufer;
	tempUploadBufer.Create(m_device.Get(), 64);*/
}

void Ideal::D3D12RayTracingRenderer::BuildShaderTables()
{
	void* rayGenShaderIdentifier;
	void* missShaderIdentifier;
	void* hitGroupShaderIdentifier;

	uint32 shaderIdentifierSize;
	auto GetShaderIdentifiers = [&](auto* stateObjectProperties)
	{
		rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_raygenShaderName);
		missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_missShaderName);
		hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_hitGroupName);
	};

	// Get shader identifiers
	{
		ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
		Check(m_dxrStateObject.As(&stateObjectProperties));
		GetShaderIdentifiers(stateObjectProperties.Get());
		shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	}

	//rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_raygenShaderName);
	//missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_missShaderName);
	//hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(m_hitGroupName);

	// Ray gen shader table
	{
		struct RootArguments
		{
			RayGenConstantBuffer cb;
		} rootArguments;
		rootArguments.cb = m_rayGenCB;

		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);

		//Ideal::D3D12UploadBuffer tempUploadBufer;
		//tempUploadBufer.Create(m_device.Get(), 64);
		Ideal::D3D12ShaderTable rayGenShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
		rayGenShaderTable.push_back(Ideal::D3D12ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
		m_rayGenShaderTable = rayGenShaderTable.GetResource();
	}

	// Miss shader table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::D3D12ShaderTable missShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(Ideal::D3D12ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource();
	}

	// Hit group shader table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::D3D12ShaderTable hitGroupShaderTable(m_device.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
		hitGroupShaderTable.push_back(Ideal::D3D12ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize));
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
	m_uavDescriptorHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
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

	// Bind the hepas, 가속구조, dispatch rays
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};

	commandlist->SetDescriptorHeaps(1, m_uavDescriptorHeap->GetDescriptorHeap().GetAddressOf());
	commandlist->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, m_raytacingOutputResourceUAVGpuDescriptorHandle.GetGpuHandle());
	commandlist->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, m_topLevelAccelerationStructure->GetGPUVirtualAddress());
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
