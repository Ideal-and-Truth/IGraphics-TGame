#include "D3D12RayTracingRenderer.h"

#include <DirectXColors.h>
#include "Misc/Utils/PIX.h"
#include "Misc/Assimp/AssimpConverter.h"

#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
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

#include <d3dx12.h>
#include <dxcapi.h>


Ideal::D3D12RayTracingRenderer::D3D12RayTracingRenderer(HWND hwnd, uint32 Width, uint32 Height, bool EditorMode)
	: m_hwnd(hwnd),
	m_width(Width),
	m_height(Height),
	m_frameIndex(0),
	m_fenceEvent(NULL),
	m_fenceValue(0)
{

}

Ideal::D3D12RayTracingRenderer::~D3D12RayTracingRenderer()
{
	Fence();
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		WaitForFenceValue(m_lastFenceValues[i]);
	}
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

	// --- Test --- //
	InitializeBLAS();
	InitializeTLAS();
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
		m_commandLists[i]->Close();
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

void Ideal::D3D12RayTracingRenderer::InitializeBLAS()
{
	std::vector<TestVertex> vertices;
	std::vector<uint32> indices;
	
	{
		indices = { 0,1,2 };
		float depthValue = 1.0;
		float offset = 0.7f;
		vertices =
		{
			{Vector3(0, -offset, depthValue), Vector4(1,0,0,1)},
			{Vector3(-offset, offset, depthValue), Vector4(0,1,0,1)},
			{Vector3(offset, offset, depthValue), Vector4(0,0,1,1)}
		};
	}

	std::shared_ptr<Ideal::D3D12VertexBuffer> vertexBuffer = std::make_shared<Ideal::D3D12VertexBuffer>();
	std::shared_ptr<Ideal::D3D12IndexBuffer> indexBuffer = std::make_shared<Ideal::D3D12IndexBuffer>();

	m_resourceManager->CreateVertexBuffer(vertexBuffer, vertices);
	m_resourceManager->CreateIndexBuffer(indexBuffer, indices);

	D3D12_RAYTRACING_GEOMETRY_DESC geometry;
	geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometry.Triangles.VertexBuffer.StartAddress = vertexBuffer->GetResource()->GetGPUVirtualAddress();
	geometry.Triangles.VertexBuffer.StrideInBytes = vertexBuffer->GetElementSize();
	geometry.Triangles.VertexCount = static_cast<uint32>(vertices.size());
	geometry.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometry.Triangles.IndexBuffer = indexBuffer->GetResource()->GetGPUVirtualAddress();
	geometry.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	geometry.Triangles.IndexCount = static_cast<uint32>(indexBuffer->GetElementCount());
	geometry.Triangles.Transform3x4 = 0;
	geometry.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;	// 지오메트리가 불투명하다면 opaque로 지정 아니면 _Flag_none으로 지정

	// blas input desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS ASInputs = {};
	ASInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	ASInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

	ASInputs.pGeometryDescs = &geometry;

	ASInputs.NumDescs = 1;

	// 예상되는 BLAS 사용, 메모리와 성능 최적화 허용을 나타냄, _FLAG_MINIMIZE_MEMORY, COMPACATION플래그는 필요한 메모리를 줄이는데 도움이 된다.
	ASInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;	

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO ASBuildInfo = {};
	m_device->GetRaytracingAccelerationStructurePrebuildInfo(&ASInputs, &ASBuildInfo);

	UploadBuffer(ASBuildInfo, blasScratch, blasResult);
	CreateAS(ASInputs, blasScratch, blasResult);
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

void Ideal::D3D12RayTracingRenderer::InitShader()
{
	CompileShader(m_library.Get(), m_compiler.Get(), L"RayGen.hlsl", &rgsBytecode);
	CompileShader(m_library.Get(), m_compiler.Get(), L"Miss.hlsl", &missBytecode);
	CompileShader(m_library.Get(), m_compiler.Get(), L"ClosestHit.hlsl", &chsBytecode);
	CompileShader(m_library.Get(), m_compiler.Get(), L"AnyHit.hlsl", &ahsBytecode);
}

void Ideal::D3D12RayTracingRenderer::CreateRTPSO()
{

	// 이건 헬퍼
	//CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };


	// -----> 여기 사이에 subObjects 넣어주기 <------//
	
	D3D12_STATE_OBJECT_DESC rtpsoDesc = {};
	rtpsoDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	rtpsoDesc.NumSubobjects = static_cast<uint32>(subobjects.size());
	rtpsoDesc.pSubobjects = subobjects.data();

	// 레이 트레이싱 파이프라인 상태 오브젝트 생성
	m_device->CreateStateObject(&rtpsoDesc, IID_PPV_ARGS(&rtpso));

	// 레이 트레이싱 파이프라인 상태 오브젝트의 속성 얻기
	rtpso->QueryInterface(IID_PPV_ARGS(&rtpsoInfo));
}

void Ideal::D3D12RayTracingRenderer::InitShaderSubobject()
{
	subobjects.push_back(CreateShaderSubobject(L"Unique_RayGenerationShader_Name", L"RayGen", rgsBytecode));
	subobjects.push_back(CreateShaderSubobject(L"Unique_Miss_Name", L"Miss", missBytecode));
	subobjects.push_back(CreateShaderSubobject(L"Unique_ClosestHitShader_Name", L"ClosestHit", chsBytecode));
	subobjects.push_back(CreateShaderSubobject(L"Unique_AnyHitShader_Name", L"AnyHit", ahsBytecode));
}

void Ideal::D3D12RayTracingRenderer::InitHitGroupSubobject()
{
	D3D12_HIT_GROUP_DESC hitGroupDesc = {};
	hitGroupDesc.ClosestHitShaderImport = L"Unique_ClosestHitShader_Name";
	hitGroupDesc.AnyHitShaderImport = L"Unique_AnyHitShader_Name";
	hitGroupDesc.IntersectionShaderImport = nullptr; // 이건 나도 모르겠음

}

D3D12_STATE_SUBOBJECT Ideal::D3D12RayTracingRenderer::CreateShaderSubobject(const wchar_t* Name, const wchar_t* EntryName, ComPtr<IDxcBlob> ShaderByteCode)
{
	// DXIL 라이브러리의 진입점과 이름을 기술한다.
	D3D12_EXPORT_DESC rgsExportDesc = {};
	// 고유한 이름(다른 곳에서 참조하기 위해)
	rgsExportDesc.Name = Name;
	// HLSL 셰이더 소스의 진입점
	rgsExportDesc.ExportToRename = EntryName;
	rgsExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

	// DXIL 라이브러리 설명
	D3D12_DXIL_LIBRARY_DESC libDesc = {};
	libDesc.DXILLibrary.BytecodeLength = ShaderByteCode->GetBufferSize();
	libDesc.DXILLibrary.pShaderBytecode = ShaderByteCode->GetBufferPointer();
	libDesc.NumExports = 1;
	libDesc.pExports = &rgsExportDesc;

	// 광선 생성 셰이더 상태 서브오브젝트 설명
	D3D12_STATE_SUBOBJECT subobject = {};
	subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	subobject.pDesc = &libDesc;
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
