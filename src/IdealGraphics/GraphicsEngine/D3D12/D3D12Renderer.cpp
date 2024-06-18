#include "D3D12Renderer.h"

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


Ideal::D3D12Renderer::D3D12Renderer(HWND hwnd, uint32 width, uint32 height, bool EditorMode)
	: m_hwnd(hwnd),
	m_width(width),
	m_height(height),
	//m_viewport,
	m_frameIndex(0),
	m_fenceEvent(NULL),
	m_fenceValue(0),
	m_rtvDescriptorSize(0),
	m_isEditor(EditorMode)
{
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

Ideal::D3D12Renderer::~D3D12Renderer()
{
	Fence();
	for (uint64 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		WaitForFenceValue(m_lastFenceValues[i]);
	}

#ifdef _DEBUG
	if (m_isEditor)
	{
		m_imguiSRVHandle.Free();
	}
#endif 

	// Release Resource Manager
	m_resourceManager = nullptr;

#ifdef _DEBUG
	if (m_isEditor)
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
#endif
}

void Ideal::D3D12Renderer::Init()
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
		//OffWarningRenderTargetClearValue();
	}
#endif

	//---------------------Viewport Init-------------------------//
	m_viewport = std::make_shared<Ideal::D3D12Viewport>(m_hwnd, m_width, m_height);
	m_viewport->Init();

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

	//------------Create Fence---------------//
	CreateFence();

	//---------------------SwapChain-------------------------//

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
	Check(factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		m_hwnd,
		&swapChainDesc,
		//nullptr,
		&fullScreenDesc,
		nullptr,
		swapChain.GetAddressOf()
	));

	Check(factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));

	Check(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	//------ImGuiSRVHeap------//
	m_imguiSRVHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
	m_imguiSRVHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 2 /*FONT*/ /*Main Camera*/);

	//---------------------RTV-------------------------//
	m_rtvHeap = std::make_shared<Ideal::D3D12DescriptorHeap>();
	m_rtvHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, SWAP_CHAIN_FRAME_COUNT);

	float c[4] = { 0.f,0.f,0.f,1.f };
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = swapChainDesc.Format;
	clearValue.Color[0] = c[0];
	clearValue.Color[1] = c[1];
	clearValue.Color[2] = c[2];
	clearValue.Color[3] = c[3];

	// descriptor heap ���� rtv Descriptor�� ũ��
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
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

	//---------------------DSV-------------------------//
	// 2024.04.14 : dsv�� ����ڴ�. ���� descriptor heap�� �����.
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	Check(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

	CreateDSV(m_width, m_height);

	//------------------Create Default Camera------------------//
	CreateDefaultCamera();
	// Temp
	//m_mainCamera->Walk(50.f);
	//m_mainCamera->SetPosition(Vector3(0.f, 100.f, -200.f));
	//m_mainCamera->UpdateViewMatrix();
	//m_mainCamera->UpdateMatrix2();

	//------------------Resource Manager---------------------//
	m_resourceManager = std::make_shared<Ideal::ResourceManager>();
	m_resourceManager->Init(m_device);
	m_resourceManager->SetAssetPath(m_assetPath);
	m_resourceManager->SetModelPath(m_modelPath);
	m_resourceManager->SetTexturePath(m_texturePath);

	//------------------Create Main Descriptor Heap---------------------//
	CreateDescriptorHeap();

	// 2024.05.31 
	//------------------Create CB Pool---------------------//
	CreateAndInitRenderingResources();


#ifdef _DEBUG
	//---------------------Editor RTV-------------------------//
	m_editorRTVHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
	m_editorRTVHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);

	CreateEditorRTV(m_width, m_height);
	//---------------------Init Imgui-----------------------//
	if (m_isEditor)
	{
		InitImGui();
	}

	//---------------------Warning OFF-----------------------//
	OffWarningRenderTargetClearValue();
#endif
}

void Ideal::D3D12Renderer::Tick()
{
	__debugbreak();
	return;
}

void Ideal::D3D12Renderer::Render()
{
	ResetCommandList();
	//---------Update Camera Matrix---------//
	m_mainCamera->UpdateMatrix2();

	//2024.05.16 Draw GBuffer
	m_currentRenderScene.lock()->DrawGBuffer(shared_from_this());

#ifdef _DEBUG
	if (m_isEditor)
	{
		SetImGuiCameraRenderTarget();
		m_currentRenderScene.lock()->DrawScreen(shared_from_this());
		DrawImGuiMainCamera();
	}
#endif
	//-------------Begin Render------------//
	BeginRender();
	{

		//-------------Render Command-------------//
		{
			//----------Render Scene-----------//
			if (!m_currentRenderScene.expired())
			{
#ifdef _DEBUG
				if (!m_isEditor)
#endif
					m_currentRenderScene.lock()->DrawScreen(shared_from_this());
			}
		}
		{
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
		}
	}
	//-------------End Render------------//
	EndRender();

	//----------------Present-------------------//
	Present();
	return;
}

void Ideal::D3D12Renderer::Resize(UINT Width, UINT Height)
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
	m_depthStencil.Reset();

	// ResizeBuffers
	Check(m_swapChain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, Width, Height, DXGI_FORMAT_R8G8B8A8_UNORM, m_swapChainFlags));

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		auto handle = m_renderTargets[i]->GetRTV();
		ComPtr<ID3D12Resource> resource = m_renderTargets[i]->GetResource();
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&resource));
		m_device->CreateRenderTargetView(resource.Get(), nullptr, handle.GetCpuHandle());

		m_renderTargets[i]->Create(resource);
		m_renderTargets[i]->EmplaceRTV(handle);
	}
	// CreateDepthStencil
	CreateDSV(Width, Height);

	m_width = Width;
	m_height = Height;

	// Viewport Reize
	m_viewport->ReSize(Width, Height);

	m_mainCamera->SetAspectRatio(float(Width) / Height);

	m_currentRenderScene.lock()->Resize(shared_from_this());

#ifdef _DEBUG
	if (m_isEditor)
	{
		ResizeImGuiMainCameraWindow(Width, Height);
	}
#endif
}

std::shared_ptr<Ideal::ICamera> Ideal::D3D12Renderer::CreateCamera()
{
	std::shared_ptr<Ideal::IdealCamera> newCamera = std::make_shared<Ideal::IdealCamera>();

	return newCamera;
}

void Ideal::D3D12Renderer::SetMainCamera(std::shared_ptr<Ideal::ICamera> Camera)
{
	m_mainCamera = std::static_pointer_cast<Ideal::IdealCamera>(Camera);
}

std::shared_ptr<Ideal::IMeshObject> Ideal::D3D12Renderer::CreateStaticMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealStaticMeshObject> newStaticMesh = std::make_shared<Ideal::IdealStaticMeshObject>();
	m_resourceManager->CreateStaticMeshObject(shared_from_this(), newStaticMesh, FileName);

	newStaticMesh->Init(shared_from_this());

	//m_staticMeshObjects.push_back(newStaticMesh);

	return newStaticMesh;
}

std::shared_ptr<Ideal::ISkinnedMeshObject> Ideal::D3D12Renderer::CreateSkinnedMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealSkinnedMeshObject> newDynamicMesh = std::make_shared<Ideal::IdealSkinnedMeshObject>();
	m_resourceManager->CreateSkinnedMeshObject(shared_from_this(), newDynamicMesh, FileName);

	newDynamicMesh->Init(shared_from_this());
	//m_dynamicMeshObjects.push_back(newDynamicMesh);

	return newDynamicMesh;
}

std::shared_ptr<Ideal::IAnimation> Ideal::D3D12Renderer::CreateAnimation(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealAnimation> newAnimation = std::make_shared<Ideal::IdealAnimation>();
	m_resourceManager->CreateAnimation(newAnimation, FileName);

	return newAnimation;
}

std::shared_ptr<Ideal::IRenderScene> Ideal::D3D12Renderer::CreateRenderScene()
{
	std::shared_ptr<Ideal::IdealRenderScene> newScene = std::make_shared<Ideal::IdealRenderScene>();
	newScene->Init(shared_from_this());
	return newScene;
}

void Ideal::D3D12Renderer::SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene)
{
	m_currentRenderScene = std::static_pointer_cast<Ideal::IdealRenderScene>(RenderScene);
}

std::shared_ptr<Ideal::IDirectionalLight> Ideal::D3D12Renderer::CreateDirectionalLight()
{
	std::shared_ptr<Ideal::IDirectionalLight> newLight = std::make_shared<Ideal::IdealDirectionalLight>();
	return newLight;
}

std::shared_ptr<Ideal::ISpotLight> Ideal::D3D12Renderer::CreateSpotLight()
{
	std::shared_ptr<Ideal::ISpotLight> newLight = std::make_shared<Ideal::IdealSpotLight>();
	return newLight;
}

std::shared_ptr<Ideal::IPointLight> Ideal::D3D12Renderer::CreatePointLight()
{
	std::shared_ptr<Ideal::IPointLight> newLight = std::make_shared<Ideal::IdealPointLight>();
	return newLight;
}

void Ideal::D3D12Renderer::ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData /*= false*/, bool NeedVertexInfo /*= false*/)
{
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName, isSkinnedData);

	// Temp : ".fbx" ����
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

void Ideal::D3D12Renderer::ConvertAnimationAssetToMyFormat(std::wstring FileName)
{
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName, false);

	// Temp : ".fbx" ����
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();

	assimpConverter->ExportAnimationData(FileName);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Ideal::D3D12Renderer::SetImGuiWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (::ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
}

void Ideal::D3D12Renderer::ClearImGui()
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

void Ideal::D3D12Renderer::Release()
{

}

Microsoft::WRL::ComPtr<ID3D12Device> Ideal::D3D12Renderer::GetDevice()
{
	return m_device;
}

void Ideal::D3D12Renderer::ResetCommandList()
{
	//Check(m_commandAllocator->Reset());
	//Check(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandAllocators[m_currentContextIndex];
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandLists[m_currentContextIndex];

	Check(commandAllocator->Reset());
	Check(commandList->Reset(commandAllocator.Get(), nullptr));
}

uint32 Ideal::D3D12Renderer::GetFrameIndex() const
{
	return m_frameIndex;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Ideal::D3D12Renderer::GetCommandList()
{
	return m_commandLists[m_currentContextIndex];
	//return m_commandList;
}

void Ideal::D3D12Renderer::CreateFence()
{
	Check(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));

	m_fenceValue = 0;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void Ideal::D3D12Renderer::CreateDSV(uint32 Width, uint32 Height)
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

std::shared_ptr<Ideal::ResourceManager> Ideal::D3D12Renderer::GetResourceManager()
{
	return m_resourceManager;
}

void Ideal::D3D12Renderer::CreateDescriptorHeap()
{
	m_descriptorHeap = std::make_shared<Ideal::D3D12DescriptorHeap>();
	m_descriptorHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_DESCRIPTOR_COUNT);
}

std::shared_ptr<Ideal::D3D12DescriptorHeap> Ideal::D3D12Renderer::GetMainDescriptorHeap()
{
	return m_descriptorHeaps[m_currentContextIndex];
	//return m_descriptorHeap;
}

std::shared_ptr<Ideal::ConstantBufferContainer> Ideal::D3D12Renderer::ConstantBufferAllocate(uint32 SizePerCB)
{
	// TEMP : Index = 0 
	// TODO : ��ø ������ �� ���� �ε����� �޾ƿ����� ����
	//return m_cbAllocator[0]->Allocate(m_device.Get(), SizePerCB);
	return m_cbAllocator[m_currentContextIndex]->Allocate(m_device.Get(), SizePerCB);
}

void Ideal::D3D12Renderer::CreateDefaultCamera()
{
	m_mainCamera = std::make_shared<Ideal::IdealCamera>();
	std::shared_ptr<Ideal::IdealCamera> camera = std::static_pointer_cast<Ideal::IdealCamera>(m_mainCamera);
	camera->SetLens(0.25f * 3.141592f, m_aspectRatio, 1.f, 3000.f);
}

DirectX::SimpleMath::Matrix Ideal::D3D12Renderer::GetView()
{
	return m_mainCamera->GetView();
}

DirectX::SimpleMath::Matrix Ideal::D3D12Renderer::GetProj()
{
	return m_mainCamera->GetProj();
}

DirectX::SimpleMath::Matrix Ideal::D3D12Renderer::GetViewProj()
{
	return m_mainCamera->GetViewProj();
}

DirectX::SimpleMath::Vector3 Ideal::D3D12Renderer::GetEyePos()
{
	return m_mainCamera->GetPosition();
}

void Ideal::D3D12Renderer::InitImGui()
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
	//m_imguiSRVHandle = m_resourceManager->GetImGuiSRVPool()->Allocate();
	m_imguiSRVHandle = m_imguiSRVHeap->Allocate();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX12_Init(m_device.Get(), SWAP_CHAIN_FRAME_COUNT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		//m_resourceManager->GetImguiSRVHeap().Get(),
		m_imguiSRVHeap->GetDescriptorHeap().Get(),
		m_imguiSRVHandle.GetCpuHandle(),
		m_imguiSRVHandle.GetGpuHandle());
}

void Ideal::D3D12Renderer::OffWarningRenderTargetClearValue()
{
	ComPtr<ID3D12InfoQueue> infoQueue;
	m_device.As(&infoQueue);
	D3D12_MESSAGE_ID denyIds[] =
	{
		D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
	};
	D3D12_MESSAGE_SEVERITY severities[] =
	{
		D3D12_MESSAGE_SEVERITY_INFO
	};
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;
	filter.DenyList.NumSeverities = _countof(severities);
	filter.DenyList.pSeverityList = severities;

	infoQueue->PushStorageFilter(&filter);

	// �Ʒ��� �ڵ�� ��� �߸� �ٷ� ����� �극��ũ �ع���.
	//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	//https://blog.techlab-xe.net/dx12-debug-id3d12infoqueue/
}

void Ideal::D3D12Renderer::DrawImGuiMainCamera()
{
	ImGui::Begin("MAIN SCREEN");                          // Create a window called "Hello, world!" and append into it.

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 size(windowSize.x, windowSize.y);
	//ImVec2 size(m_width/4, m_height/4);

	m_width = windowSize.x;
	m_height = windowSize.y;
	m_aspectRatio = float(m_width) / m_height;
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

void Ideal::D3D12Renderer::SetImGuiCameraRenderTarget()
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

void Ideal::D3D12Renderer::ResizeImGuiMainCameraWindow(uint32 Width, uint32 Height)
{
	CreateEditorRTV(Width, Height);
}

void Ideal::D3D12Renderer::CreateEditorRTV(uint32 Width, uint32 Height)
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

void Ideal::D3D12Renderer::CreateAndInitRenderingResources()
{
	// CreateCommand List
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; ++i)
	{
		HRESULT hr;
		hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocators[i].GetAddressOf()));
		Check(hr);
		hr = m_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocators[i].Get(),
			nullptr,
			IID_PPV_ARGS(m_commandLists[i].GetAddressOf())
		);
		Check(hr);
		m_commandLists[i]->Close();
	}

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

uint64 Ideal::D3D12Renderer::Fence()
{
	m_fenceValue++;
	m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
	m_lastFenceValues[m_currentContextIndex] = m_fenceValue;

	return m_fenceValue;
}

void Ideal::D3D12Renderer::BeginRender()
{
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandAllocators[m_currentContextIndex];
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandLists[m_currentContextIndex];

	//Check(commandAllocator->Reset());
	//Check(commandList->Reset(commandAllocator.Get(), nullptr));

	CD3DX12_RESOURCE_BARRIER backBufferBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex]->GetResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &backBufferBarrier);

	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	auto rtvHandle = m_renderTargets[m_frameIndex]->GetRTV();


	commandList->ClearRenderTargetView(rtvHandle.GetCpuHandle(), DirectX::Colors::Black, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->RSSetViewports(1, &m_viewport->GetViewport());
	commandList->RSSetScissorRects(1, &m_viewport->GetScissorRect());
	commandList->OMSetRenderTargets(1, &rtvHandle.GetCpuHandle(), FALSE, &dsvHandle);
}

void Ideal::D3D12Renderer::EndRender()
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

void Ideal::D3D12Renderer::Present()
{
	Fence();

	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	Check(hr);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	uint32 nextContextIndex = (m_currentContextIndex + 1) % MAX_PENDING_FRAME_COUNT;
	WaitForFenceValue(m_lastFenceValues[nextContextIndex]);

	m_descriptorHeaps[nextContextIndex]->Reset();
	m_cbAllocator[nextContextIndex]->Reset();

	m_currentContextIndex = nextContextIndex;
}

void Ideal::D3D12Renderer::WaitForFenceValue(uint64 ExpectedFenceValue)
{
	if (m_fence->GetCompletedValue() < ExpectedFenceValue)
	{
		m_fence->SetEventOnCompletion(ExpectedFenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}
