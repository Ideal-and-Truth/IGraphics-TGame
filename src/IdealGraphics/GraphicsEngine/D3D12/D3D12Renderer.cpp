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

#include "GraphicsEngine/Resource/IdealCamera.h"
#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealAnimation.h"
#include "GraphicsEngine/Resource/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealRenderScene.h"
#include "GraphicsEngine/Resource/IdealScreenQuad.h"

#include "GraphicsEngine/Resource/Light/IdealDirectionalLight.h"
#include "GraphicsEngine/Resource/Light/IdealSpotLight.h"
#include "GraphicsEngine/Resource/Light/IdealPointLight.h"

Ideal::D3D12Renderer::D3D12Renderer(HWND hwnd, uint32 width, uint32 height)
	: m_hwnd(hwnd),
	m_width(width),
	m_height(height),
	//m_viewport,
	m_frameIndex(0),
	m_graphicsFenceEvent(NULL),
	m_graphicsFenceValue(0),
	m_rtvDescriptorSize(0)
{
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

Ideal::D3D12Renderer::~D3D12Renderer()
{
	// Release Resource Manager
	m_resourceManager = nullptr;
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

	//------------------Create Default Camera------------------//
	CreateDefaultCamera();
	// Temp
	//m_mainCamera->Walk(50.f);
	m_mainCamera->SetPosition(Vector3(0.f, 100.f, -200.f));
	//m_mainCamera->UpdateViewMatrix();
	//m_mainCamera->UpdateMatrix2();

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
	CreateDescriptorHeap();
	//------------------Create CB Pool---------------------//
	CreateCBPool();

	// 2024.05.14 : MRT Test
	//m_resourceManager->CreateEmptyTexture2D(t1, m_width, m_height, true);
	/*for (uint32 i = 0; i < m_gBufferCount; ++i)
	{
		std::shared_ptr<Ideal::D3D12Texture> gBuffer = nullptr;
		m_resourceManager->CreateEmptyTexture2D(gBuffer, m_width, m_height, true);
		m_gBuffers.push_back(gBuffer);
	}*/

	//---------------------Init Imgui-----------------------//
	// 2024.05.22
	InitImgui();

}

void Ideal::D3D12Renderer::Tick()
{
	float speed = 2.f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		speed = 0.2f;
	}
	if (GetAsyncKeyState('O') & 0x8000)
	{
		m_mainCamera = std::static_pointer_cast<Ideal::IdealCamera>(c1);
	}
	if (GetAsyncKeyState('P') & 0x8000)
	{
		m_mainCamera = std::static_pointer_cast<Ideal::IdealCamera>(c2);
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_mainCamera->Walk(speed);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_mainCamera->Walk(-speed);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_mainCamera->Strafe(-speed);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_mainCamera->Strafe(speed);
	}
	if (GetAsyncKeyState('L') & 0x8000)
	{
		m_mainCamera->SetLook(Vector3(0.f,1.f,1.f));
	}
	if (GetAsyncKeyState('K') & 0x8000)
	{
		m_mainCamera->SetLook(Vector3(0.f, 0.f, -1.f));
	}
	if (GetAsyncKeyState('J') & 0x8000)
	{
		m_mainCamera->SetLook(Vector3(0.f, 0.f, 1.f));
	}
}

void Ideal::D3D12Renderer::Render()
{
	//------IMGUI BEGIN------//
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		//------IMGUI Tick------//
		ImGuiIO& io = ImGui::GetIO();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}

	ResetCommandList();
	//---------Update Camera Matrix---------//
	m_mainCamera->UpdateMatrix2();

	//2024.05.16 Draw GBuffer
	m_currentRenderScene->DrawGBuffer(shared_from_this());

	//-------------Begin Render------------//
	BeginRender();
	{

		//-------------Render Command-------------//
		{
			//----------Render Scene-----------//
			if (m_currentRenderScene != nullptr)
			{
				//m_currentRenderScene->Draw(shared_from_this());
				m_currentRenderScene->DrawScreen(shared_from_this());
			}
		}
		{
			//------IMGUI RENDER------//
			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList.Get());
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
	m_cb2048Pool->Reset();
	m_cbBonePool->Reset();
	return;
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

	assimpConverter->ReadAssetFile(FileName);

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

void Ideal::D3D12Renderer::ConvertAnimationAssetToMyFormat(std::wstring FileName)
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Ideal::D3D12Renderer::SetImGuiWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (::ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
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
	Check(m_commandAllocator->Reset());
	Check(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
}

void Ideal::D3D12Renderer::BeginRender()
{
	// 2024.04.15 pipeline state를 m_currentPipelineState에 있는 것으로 세팅한다.

	m_commandList->RSSetViewports(1, &m_viewport->GetViewport());
	m_commandList->RSSetScissorRects(1, &m_viewport->GetScissorRect());

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
	//m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	m_commandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::DimGray, 0, nullptr);
	// 2024.04.14 Clear DSV
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void Ideal::D3D12Renderer::EndRender()
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

void Ideal::D3D12Renderer::GraphicsPresent()
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

uint32 Ideal::D3D12Renderer::GetFrameIndex() const
{
	return m_frameIndex;
}

void Ideal::D3D12Renderer::CreateCommandList()
{
	Check(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	Check(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
	m_commandList->Close();
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Ideal::D3D12Renderer::GetCommandList()
{
	return m_commandList;
}

void Ideal::D3D12Renderer::CreateGraphicsFence()
{
	Check(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_graphicsFence.GetAddressOf())));

	m_graphicsFenceValue = 0;

	m_graphicsFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

uint64 Ideal::D3D12Renderer::GraphicsFence()
{
	m_graphicsFenceValue++;
	m_commandQueue->Signal(m_graphicsFence.Get(), m_graphicsFenceValue);
	return m_graphicsFenceValue;
}

void Ideal::D3D12Renderer::WaitForGraphicsFenceValue()
{
	const uint64 expectedFenceValue = m_graphicsFenceValue;

	if (m_graphicsFence->GetCompletedValue() < expectedFenceValue)
	{
		m_graphicsFence->SetEventOnCompletion(expectedFenceValue, m_graphicsFenceEvent);
		WaitForSingleObject(m_graphicsFenceEvent, INFINITE);
	}
}

std::shared_ptr<Ideal::ResourceManager> Ideal::D3D12Renderer::GetResourceManager()
{
	return m_resourceManager;
}

void Ideal::D3D12Renderer::CreateDescriptorHeap()
{
	m_descriptorHeap = std::make_shared<Ideal::D3D12DescriptorHeap>();
	m_descriptorHeap->Create(shared_from_this(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, MAX_DESCRIPTOR_COUNT);
}

std::shared_ptr<Ideal::D3D12DescriptorHeap> Ideal::D3D12Renderer::GetMainDescriptorHeap()
{
	return m_descriptorHeap;
}

void Ideal::D3D12Renderer::CreateCBPool()
{
	m_cb256Pool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cb256Pool->Init(m_device.Get(), 256, MAX_DRAW_COUNT_PER_FRAME);

	m_cb512Pool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cb512Pool->Init(m_device.Get(), 512, MAX_DRAW_COUNT_PER_FRAME);

	m_cb1024Pool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cb1024Pool->Init(m_device.Get(), 1024, MAX_DRAW_COUNT_PER_FRAME);

	m_cb2048Pool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cb2048Pool->Init(m_device.Get(), 2048, MAX_DRAW_COUNT_PER_FRAME);

	// TEMP
	m_cbBonePool = std::make_shared<Ideal::D3D12ConstantBufferPool>();
	m_cbBonePool->Init(m_device.Get(), AlignConstantBufferSize(static_cast<uint32>(sizeof(CB_Bone))), MAX_DRAW_COUNT_PER_FRAME);
}

std::shared_ptr<Ideal::D3D12ConstantBufferPool> Ideal::D3D12Renderer::GetCBPool(uint32 SizePerCB)
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
	else if (SizePerCB <= 2048)
	{
		return m_cb2048Pool;
	}
	return nullptr;
}

std::shared_ptr<Ideal::D3D12ConstantBufferPool> Ideal::D3D12Renderer::GetCBBonePool()
{
	return m_cbBonePool;
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

void Ideal::D3D12Renderer::InitImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	//-----Allocate Srv-----//
	auto handle = m_resourceManager->GetImguiSRVPool()->Allocate();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX12_Init(m_device.Get(), 2,
		DXGI_FORMAT_R8G8B8A8_UNORM, m_resourceManager->GetImguiSRVHeap().Get(),
		handle.GetCpuHandle(),
		handle.GetGpuHandle());
}
