#include "D3D12Renderer.h"
#include "GraphicsEngine/VertexInfo.h"
#include <DirectXColors.h>
#include "Misc/Utils/PIX.h"


// Test
#include "Misc/Assimp/AssimpConverter.h"
#include "ThirdParty/Include/DirectXTK12/WICTextureLoader.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12RootSignature.h"
#include "GraphicsEngine/Resource/Camera.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/Refactor/IdealAnimation.h"
#include "GraphicsEngine/Resource/Refactor/IdealSkinnedMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/Refactor/IdealRenderScene.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"

D3D12Renderer::D3D12Renderer(HWND hwnd, uint32 width, uint32 height)
	: m_hwnd(hwnd),
	m_width(width),
	m_height(height),
	m_viewport(hwnd, width, height),
	m_frameIndex(0),
	m_graphicsFenceEvent(NULL),
	m_graphicsFenceValue(0),
	m_rtvDescriptorSize(0)
{
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

D3D12Renderer::~D3D12Renderer()
{
	// Release Resource Manager
	m_resourceManager = nullptr;
}

void D3D12Renderer::Init()
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
	m_viewport.Init();

	//---------------------Create Device-------------------------//

	ComPtr<IDXGIFactory4> factory;
	Check(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf())));

	D3D_FEATURE_LEVEL	featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DWORD	FeatureLevelNum = _countof(featureLevels);
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	DXGI_ADAPTER_DESC1 adapterDesc = {};
	for (DWORD featerLevelIndex = 0; featerLevelIndex < FeatureLevelNum; featerLevelIndex++)
	{
		UINT adapterIndex = 0;
		while (DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter))
		{
			adapter->GetDesc1(&adapterDesc);

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
	//ComPtr<IDXGIAdapter1> hardwareAdapter;
	//Check(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.GetAddressOf())));


	//---------------------Command Queue-------------------------//

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	Check(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

	//------------Create Command List-------------//
	CreateCommandList();

	//------------Create Fence---------------//
	CreateGraphicsFence();

	//---------------------SwapChain-------------------------//

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FRAME_BUFFER_COUNT;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	Check(factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		m_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain.GetAddressOf()
	));

	Check(factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));

	Check(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	//---------------------RTV-------------------------//

	// rtv descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	Check(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

	// descriptor heap 에서 rtv Descriptor의 크기
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (uint32 i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			Check(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
			m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	//---------------------DSV-------------------------//

	// 2024.04.14 : dsv를 만들겠다. 먼저 descriptor heap을 만든다.
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	Check(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

	{
		D3D12_CLEAR_VALUE depthClearValue = {};
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			m_width,
			m_height,
			1,
			0,
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

		m_device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(m_depthStencil.GetAddressOf())
		);

		// create dsv
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_device->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	// 2024.04.21 Ideal::Descriptor Heap
	m_idealSrvHeap.Create(shared_from_this(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, m_srvHeapNum);

	//------------------Create Default Camera------------------//
	CreateDefaultCamera();
	// Temp
	m_mainCamera->Walk(50.f);
	m_mainCamera->UpdateViewMatrix();

	c1 = CreateCamera();
	c1->SetLens(0.25f * 3.141592f, m_aspectRatio, 1.f, 3000.f);
	c2 = CreateCamera();
	c2->SetLens(0.25f * 3.141592f, m_aspectRatio, 1.f, 3000.f);

	//------------------Resource Manager---------------------//
	m_resourceManager = std::make_shared<Ideal::ResourceManager>();
	m_resourceManager->Init(m_device);
	m_resourceManager->SetAssetPath(m_assetPath);
	m_resourceManager->SetModelPath(m_modelPath);
	m_resourceManager->SetTexturePath(m_texturePath);

	//------------------Create Main Descriptor Heap---------------------//
	CreateDescriptorTable();
	//------------------Create CB Pool---------------------//
	CreateCBPool();
}

void D3D12Renderer::Tick()
{
	const float speed = 2.f;
	if (GetAsyncKeyState('O') & 0x8000)
	{
		m_mainCamera = std::static_pointer_cast<Ideal::Camera>(c1);
	}
	if (GetAsyncKeyState('P') & 0x8000)
	{
		m_mainCamera = std::static_pointer_cast<Ideal::Camera>(c2);
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_mainCamera->Walk(-speed);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_mainCamera->Walk(speed);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_mainCamera->Strafe(-speed);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_mainCamera->Strafe(speed);
	}
}

void D3D12Renderer::Render()
{
	m_mainCamera->UpdateViewMatrix();

	//-------------Begin Render------------//
	BeginRender();

	//-------------Render Command-------------//
	{
		//ID3D12DescriptorHeap* heaps[] = { m_resourceManager->GetSRVHeap().Get() };
		//m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);

		//----------Render Scene-----------//
		if (m_currentRenderScene != nullptr)
		{
			m_currentRenderScene->Draw(shared_from_this());
		}
	}

	//-------------End Render------------//
	EndRender();

	//----------------Present-------------------//
	GraphicsPresent();

	//-----------Reset Pool-----------//
	m_descriptorHeap->Reset();
	m_cb256Pool->Reset();
	m_cb512Pool->Reset();
	m_cb1024Pool->Reset();
	m_cbBonePool->Reset();
	return;
}

std::shared_ptr<Ideal::ICamera> D3D12Renderer::CreateCamera()
{
	std::shared_ptr<Ideal::Camera> newCamera = std::make_shared<Ideal::Camera>();

	return newCamera;
}

void D3D12Renderer::SetMainCamera(std::shared_ptr<Ideal::ICamera> Camera)
{
	m_mainCamera = std::static_pointer_cast<Ideal::Camera>(Camera);
}

std::shared_ptr<Ideal::IMeshObject> D3D12Renderer::CreateStaticMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealStaticMeshObject> newStaticMesh = std::make_shared<Ideal::IdealStaticMeshObject>();
	m_resourceManager->CreateStaticMeshObject(shared_from_this(), newStaticMesh, FileName);

	newStaticMesh->Init(shared_from_this());

	//m_staticMeshObjects.push_back(newStaticMesh);

	return newStaticMesh;
}

std::shared_ptr<Ideal::ISkinnedMeshObject> D3D12Renderer::CreateSkinnedMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealSkinnedMeshObject> newDynamicMesh = std::make_shared<Ideal::IdealSkinnedMeshObject>();
	m_resourceManager->CreateSkinnedMeshObject(shared_from_this(), newDynamicMesh, FileName);

	newDynamicMesh->Init(shared_from_this());
	//m_dynamicMeshObjects.push_back(newDynamicMesh);

	return newDynamicMesh;
}

std::shared_ptr<Ideal::IAnimation> D3D12Renderer::CreateAnimation(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealAnimation> newAnimation = std::make_shared<Ideal::IdealAnimation>();
	m_resourceManager->CreateAnimation(newAnimation, FileName);

	return newAnimation;
}

std::shared_ptr<Ideal::IRenderScene> D3D12Renderer::CreateRenderScene()
{
	std::shared_ptr<Ideal::IdealRenderScene> newScene = std::make_shared<Ideal::IdealRenderScene>();
	newScene->Init(shared_from_this());

	return newScene;
}

void D3D12Renderer::SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene)
{
	m_currentRenderScene = std::static_pointer_cast<Ideal::IdealRenderScene>(RenderScene);
}

void D3D12Renderer::Release()
{

}

void D3D12Renderer::CreateDefaultCamera()
{
	m_mainCamera = std::make_shared<Ideal::Camera>();
	std::shared_ptr<Ideal::Camera> camera = std::static_pointer_cast<Ideal::Camera>(m_mainCamera);
	camera->SetLens(0.25f * 3.141592f, m_aspectRatio, 1.f, 3000.f);
}

void D3D12Renderer::BeginRender()
{
	Check(m_commandAllocator->Reset());
	Check(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
	// 2024.04.15 pipeline state를 m_currentPipelineState에 있는 것으로 세팅한다.

	m_commandList->RSSetViewports(1, &m_viewport.GetViewport());
	m_commandList->RSSetScissorRects(1, &m_viewport.GetScissorRect());

	CD3DX12_RESOURCE_BARRIER backBufferRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	m_commandList->ResourceBarrier(1, &backBufferRenderTarget);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	// 2024.04.14 dsv세팅
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	m_commandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::DimGray, 0, nullptr);
	// 2024.04.14 Clear DSV
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void D3D12Renderer::EndRender()
{
	CD3DX12_RESOURCE_BARRIER backBufferPresent = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	m_commandList->ResourceBarrier(1, &backBufferPresent);

	Check(m_commandList->Close());
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

std::shared_ptr<Ideal::ResourceManager> D3D12Renderer::GetResourceManager()
{
	return m_resourceManager;
}

Microsoft::WRL::ComPtr<ID3D12Device> D3D12Renderer::GetDevice()
{
	return m_device;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> D3D12Renderer::GetCommandList()
{
	return m_commandList;
}

uint32 D3D12Renderer::GetFrameIndex() const
{
	return m_frameIndex;
}

void D3D12Renderer::CreateCommandList()
{
	Check(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	Check(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
	m_commandList->Close();
}

void D3D12Renderer::CreateGraphicsFence()
{
	Check(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_graphicsFence.GetAddressOf())));

	m_graphicsFenceValue = 0;

	m_graphicsFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

uint64 D3D12Renderer::GraphicsFence()
{
	m_graphicsFenceValue++;
	m_commandQueue->Signal(m_graphicsFence.Get(), m_graphicsFenceValue);
	return m_graphicsFenceValue;
}

void D3D12Renderer::WaitForGraphicsFenceValue()
{
	const uint64 expectedFenceValue = m_graphicsFenceValue;

	if (m_graphicsFence->GetCompletedValue() < expectedFenceValue)
	{
		m_graphicsFence->SetEventOnCompletion(expectedFenceValue, m_graphicsFenceEvent);
		WaitForSingleObject(m_graphicsFenceEvent, INFINITE);
	}
}

void D3D12Renderer::GraphicsPresent()
{
	HRESULT hr = m_swapChain->Present(1, 0);
	if (DXGI_ERROR_DEVICE_REMOVED == hr)
	{
		__debugbreak();
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	GraphicsFence();
	WaitForGraphicsFenceValue();
}

void D3D12Renderer::CreateDescriptorTable()
{
	m_descriptorHeap = std::make_shared<Ideal::D3D12DescriptorHeap>();
	//m_descriptorHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_DESCRIPTOR_COUNT);
	m_descriptorHeap->Create(shared_from_this(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_DESCRIPTOR_COUNT);
}

std::shared_ptr<Ideal::D3D12DescriptorHeap> D3D12Renderer::GetMainDescriptorHeap()
{
	return m_descriptorHeap;
}

void D3D12Renderer::CreateCBPool()
{
	m_cb256Pool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cb256Pool->Init(m_device.Get(), 256, MAX_DRAW_COUNT_PER_FRAME);

	m_cb512Pool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cb512Pool->Init(m_device.Get(), 512, MAX_DRAW_COUNT_PER_FRAME);

	m_cb1024Pool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cb1024Pool->Init(m_device.Get(), 1024, MAX_DRAW_COUNT_PER_FRAME);

	// TEMP
	m_cbBonePool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cbBonePool->Init(m_device.Get(), AlignConstantBufferSize(sizeof(CB_Bone)), MAX_DRAW_COUNT_PER_FRAME);
}

std::shared_ptr<Ideal::D3D12ConstantBufferPool> D3D12Renderer::GetCBPool(uint32 SizePerCB)
{
	if (SizePerCB <= 256)
	{
		return m_cb256Pool;
	}
	else if (SizePerCB <= 512)
	{
		return m_cb512Pool;
	}
	else if (SizePerCB <= 1024)
	{
		return m_cb1024Pool;
	}
	return nullptr;
}

std::shared_ptr<Ideal::D3D12ConstantBufferPool> D3D12Renderer::GetCBBonePool()
{
	return m_cbBonePool;
}

void D3D12Renderer::ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData /*= false*/)
{
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName);

	// Temp : ".fbx" 삭제
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();

	assimpConverter->ExportModelData(FileName, isSkinnedData);
	assimpConverter->ExportMaterialData(FileName);
}

void D3D12Renderer::ConvertAnimationAssetToMyFormat(std::wstring FileName)
{
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName);

	// Temp : ".fbx" 삭제
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();

	assimpConverter->ExportAnimationData(FileName);
}