#include "D3D12Renderer.h"
#include "GraphicsEngine/VertexInfo.h"
#include <DirectXColors.h>
#include "Misc/Utils/PIX.h"

#include "GraphicsEngine/Resource/Mesh.h"
#include "GraphicsEngine/Resource/ResourceManager.h"

// Test
#include "Misc/Assimp/AssimpConverter.h"
#include "GraphicsEngine/Resource/Model.h"
#include "ThirdParty/Include/DirectXTK12/WICTextureLoader.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12ResourceManager.h"
#include "GraphicsEngine/Resource/Camera.h"
#include "GraphicsEngine/Resource/MeshObject.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMeshObject.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12RootSignature.h"

D3D12Renderer::D3D12Renderer(HWND hwnd, uint32 width, uint32 height)
	: m_hwnd(hwnd),
	m_width(width),
	m_height(height),
	m_viewport(hwnd, width, height)
{
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

D3D12Renderer::~D3D12Renderer()
{
	// Release Resource Manager
	m_d3dResourceManager = nullptr;
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

	// descriptor heap ���� rtv Descriptor�� ũ��
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

	// 2024.04.14 : dsv�� ����ڴ�. ���� descriptor heap�� �����.
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
	m_idealSrvHeap.Create(shared_from_this(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_srvHeapNum);

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

	m_d3dResourceManager = std::make_shared<Ideal::D3D12ResourceManager>();
	m_d3dResourceManager->Init(m_device);
	m_d3dResourceManager->SetAssetPath(m_assetPath);
	m_d3dResourceManager->SetModelPath(m_modelPath);
	m_d3dResourceManager->SetTexturePath(m_texturePath);

	// Test
	CreatePSO();

	// Test
	//m_boxVB = std::make_shared<Ideal::D3D12VertexBuffer>();
	//m_resourceManager->CreateVertexBufferBox(m_boxVB);
	//TestCreateVertexBuffer(m_testVB);
	// Test
	//m_boxIB = std::make_shared<Ideal::D3D12IndexBuffer>();
	//m_resourceManager->CreateIndexBufferBox(m_boxIB);

	LoadAssets();
	//LoadBox();
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

	//BoxTick();
	
	m_mainCamera->UpdateViewMatrix();

	for (auto m : m_models)
	{
		m->Tick(shared_from_this());
	}
}

void D3D12Renderer::Render()
{
	//-------------Begin Render------------//
	BeginRender();

	//-------------Render Command-------------//
	{
		ID3D12DescriptorHeap* heaps[] = { m_d3dResourceManager->GetSRVHeap().Get() };
		m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);

		//DrawBox();
		// Test Models Render
		/*for (auto m : m_meshes)
		{
			m->Render(shared_from_this());
		}*/

		//----------Static Mesh-----------//
		RenderTest();
		// pipeline
		// rootsignature
		// constantBuffer
		/*for (auto m : m_meshObjects)
		{
			m->Draw(shared_from_this());
		}*/
	}

	//-------------End Render------------//
	EndRender();

	//----------------Present-------------------//
	GraphicsPresent();

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

std::shared_ptr<Ideal::IMeshObject> D3D12Renderer::CreateMeshObject(const std::wstring FileName)
{
	std::shared_ptr<Ideal::Model> model = CreateModel(FileName);
	std::shared_ptr<Ideal::MeshObject> outMeshObject = std::make_shared<Ideal::MeshObject>();
	outMeshObject->SetModel(model);

	m_meshes.push_back(outMeshObject);
	return outMeshObject;
}

std::shared_ptr<Ideal::Model> D3D12Renderer::CreateModel(const std::wstring FileName)
{
	std::shared_ptr<Ideal::Model> newModel;

	// ���� �ִٸ� ���� ���� �ʿ���� �����ش�.
	newModel = m_resourceManager->GetModel(FileName);
	if (newModel != nullptr)
	{
		return newModel;
	}

	// ���ٸ� ����� ���ҽ��Ŵ������� �־��ش�.
	newModel = std::make_shared<Ideal::Model>();
	std::wstring modelPath = m_modelPath + FileName;
	newModel->ReadModel(modelPath);

	std::wstring materialPath = m_texturePath + FileName;
	newModel->ReadMaterial(materialPath);

	newModel->Create(shared_from_this());
	m_resourceManager->AddModel(FileName, newModel);

	m_models.push_back(newModel);

	return newModel;
}

std::shared_ptr<Ideal::IMeshObject> D3D12Renderer::CreateStaticMeshObject(const std::wstring& FileName)
{
	std::shared_ptr<Ideal::IdealStaticMeshObject> newStaticMesh = std::make_shared<Ideal::IdealStaticMeshObject>();
	m_d3dResourceManager->CreateStaticMeshObject(shared_from_this(), newStaticMesh, FileName);

	newStaticMesh->Init(shared_from_this());

	m_meshObjects.push_back(newStaticMesh);

	return newStaticMesh;
}

void D3D12Renderer::CreateRootSignature()
{

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

void D3D12Renderer::LoadAssets()
{
	////////////////////////////////////// Load ASSET
	// 2024.04.18 Convert to my format
	//std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();

	//assimpConverter->ReadAssetFile(L"statue_chronos/statue_join.fbx");
	//assimpConverter->ExportModelData(L"statue_chronos/statue_chronos");
	//assimpConverter->ExportMaterialData(L"statue_chronos/statue_chronos");
	//assimpConverter.reset();

	//assimpConverter = std::make_shared<AssimpConverter>();
	//assimpConverter->ReadAssetFile(L"Tank/Tank.fbx");
	//assimpConverter->ExportModelData(L"Tank/Tank");
	//assimpConverter->ExportMaterialData(L"Tank/Tank");
	//assimpConverter.reset();
	//
	//assimpConverter = std::make_shared<AssimpConverter>();
	//assimpConverter->ReadAssetFile(L"House2/untitled.fbx");
	//assimpConverter->ExportModelData(L"House2/House2");
	//assimpConverter->ExportMaterialData(L"House2/House2");
	//assimpConverter.reset();

	//m_model = std::make_shared<Ideal::Model>();
	//m_model->ReadModel(L"porsche/porsche");	// mesh �ۿ� ����.
	//m_model->ReadMaterial(L"porsche/porsche");

	//CreateMeshObject(L"Tower/Tower");
	//CreateMeshObject(L"Tank/Tank");
	//CreateMeshObject(L"statue_chronos/statue_chronos");
	
	//CreateMeshObject(L"House2/House2");
	//LoadBox();
	//m_commandList->Close();
}

//void D3D12Renderer::CreateMeshObject(const std::wstring FileName)
//{
//	std::shared_ptr<Ideal::Model> model = std::make_shared<Ideal::Model>();
//	model->ReadModel(FileName);
//	model->ReadMaterial(FileName);
//	model->Create(shared_from_this());
//	m_models.push_back(model);
//}

void D3D12Renderer::BeginRender()
{
	Check(m_commandAllocator->Reset());
	Check(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
	// 2024.04.15 pipeline state�� m_currentPipelineState�� �ִ� ������ �����Ѵ�.

	m_commandList->RSSetViewports(1, &m_viewport.GetViewport());
	m_commandList->RSSetScissorRects(1, &m_viewport.GetScissorRect());

	CD3DX12_RESOURCE_BARRIER backBufferRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	m_commandList->ResourceBarrier(1, &backBufferRenderTarget);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	// 2024.04.14 dsv����
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

std::shared_ptr<Ideal::D3D12ResourceManager> D3D12Renderer::GetResourceManager()
{
	return m_d3dResourceManager;
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

void D3D12Renderer::CreatePSO()
{

	//-------------------Sampler--------------------//
	CD3DX12_STATIC_SAMPLER_DESC sampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);
	//-------------------Range--------------------//
	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//-------------------Parameter--------------------//
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];
	rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

	//-------------------Signature--------------------//
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Check(GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_testRootSignature.GetAddressOf())
	));

	m_staticMeshPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	m_staticMeshPSO->SetInputLayout(BasicVertex::InputElements, BasicVertex::InputElementCount);

	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	vs->CompileFromFile(L"../Shaders/BoxUV.hlsl", nullptr, nullptr, "VS", "vs_5_0");
	m_staticMeshPSO->SetVertexShader(vs);

	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	ps->CompileFromFile(L"../Shaders/BoxUV.hlsl", nullptr, nullptr, "PS", "ps_5_0");
	m_staticMeshPSO->SetPixelShader(ps);

	m_staticMeshPSO->SetRootSignature(m_testRootSignature.Get());
	m_staticMeshPSO->SetRasterizerState();
	m_staticMeshPSO->SetBlendState();
	m_staticMeshPSO->Create(shared_from_this());


	//--------------CB---------------//
	{
		const uint32 bufferSize = sizeof(Transform);

		m_constantBuffer.Create(GetDevice().Get(), bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
	}


}

void D3D12Renderer::RenderTest()
{

	for (auto& m : m_meshObjects)
	{
		// static mesh
		m_commandList->SetPipelineState(m_staticMeshPSO->GetPipelineState().Get());
		m_commandList->SetGraphicsRootSignature(m_testRootSignature.Get());
		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m->Draw(shared_from_this());
	}
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

//-------------------------------------------BOX-------------------------------------------//

void D3D12Renderer::LoadBox()
{
	{
		// CBV�� �ٽ� ��������� Root Signature�� �ٽ� �����.
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		// 2024.04.13 : ��Ʈ �ñ״��� ���� �ٲ㺸�ڴ�
		// 1�� �ְ�, b0�� �� ���̰�, data_static�� �Ϲ������� ��� ���� ������ �ִ� ���̶�� �Ѵ�(?), �׸��� �� �����ʹ� vertex ���̴����� ����� ���̴�
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);


		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		Check(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_boxRootSignature.GetAddressOf())));
	}

	CreateBoxTexPipeline();
	CreateBoxTexture();

	// 2024.04.13 : ConstantBuffer �ٽ� �����.
	// ������ ���� ��ŭ �޸𸮸� �Ҵ� �� ���̴�.
	{
		const uint32 testConstantBufferSize = sizeof(SimpleBoxConstantBuffer);

		m_boxCB.Create(m_device.Get(), testConstantBufferSize, FRAME_BUFFER_COUNT);

		m_simpleBoxConstantBufferDataBegin = (SimpleBoxConstantBuffer*)m_boxCB.GetMappedMemory(m_frameIndex);
	}
	return;
}

void D3D12Renderer::DrawBox()
{
	m_commandList->SetPipelineState(m_boxPipeline.Get());

	//---------------------IA--------------------//
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	auto vbView = m_boxVB->GetView();
	m_commandList->IASetVertexBuffers(0, 1, &vbView);
	auto ibView = m_boxIB->GetView();
	m_commandList->IASetIndexBuffer(&ibView);

	//---------------------Root Signature--------------------//
	m_commandList->SetGraphicsRootSignature(m_boxRootSignature.Get());

	ID3D12DescriptorHeap* heaps[] = { m_d3dResourceManager->GetSRVHeap().Get() };
	m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);

	m_commandList->SetGraphicsRootDescriptorTable(0, m_boxTexture->GetDescriptorHandle().GetGpuHandle());
	m_commandList->SetGraphicsRootConstantBufferView(1, m_boxCB.GetGPUVirtualAddress(m_frameIndex));

	//---------------------Draw--------------------//
	m_commandList->DrawIndexedInstanced(m_boxIB->GetElementCount(), 1, 0, 0, 0);
}

void D3D12Renderer::BoxTick()
{
	// 2024.04.14 constantBuffer ����
	static float rot = 0.0f;
	m_simpleBoxConstantBufferDataBegin = (SimpleBoxConstantBuffer*)m_boxCB.GetMappedMemory(m_frameIndex);
	Matrix mat = Matrix::CreateRotationY(rot) * Matrix::CreateRotationX(-rot) * m_mainCamera->GetViewProj();
	Matrix tMat = mat.Transpose();
	rot += 0.01f;
	m_simpleBoxConstantBufferDataBegin->worldViewProjection = tMat;
}

void D3D12Renderer::CreateBoxTexPipeline()
{
	// LinearWrap
	CD3DX12_STATIC_SAMPLER_DESC sampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	// Root Signature
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	CD3DX12_DESCRIPTOR_RANGE1 descRange[1];
	descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER1 rootParameter[2];
	rootParameter[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameter[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc(2, rootParameter, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSigDesc, featureData.HighestVersion, &signature, &error));
	Check(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_boxRootSignature.GetAddressOf())));

	ComPtr<ID3DBlob> errorBlob;
	uint32 compileFlags = 0;

	D3D12_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// Set VS
	ComPtr<ID3DBlob> vertexShader;
	Check(D3DCompileFromFile(
		L"Shaders/SimpleVertexShaderUV.hlsl",
		nullptr,
		nullptr,
		"VS",
		"vs_5_0",
		compileFlags, 0, &vertexShader, &errorBlob));

	// Set PS
	ComPtr<ID3DBlob> pixelShader;
	Check(D3DCompileFromFile(
		L"Shaders/SimpleVertexShaderUV.hlsl",
		nullptr,
		nullptr,
		"PS",
		"ps_5_0",
		compileFlags, 0, &pixelShader, &errorBlob));


	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	// Set Input Layout
	psoDesc.InputLayout = { inputElements, 2 };

	psoDesc.pRootSignature = m_boxRootSignature.Get();

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;


	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	Check(m_device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(m_boxPipeline.GetAddressOf())
	));
}

void D3D12Renderer::CreateBoxTexture()
{
	m_boxTexture = std::make_shared<Ideal::D3D12Texture>();
	std::wstring path = L"Resources/Test/test2.png";
	m_d3dResourceManager->CreateTexture(m_boxTexture, path);
	return;
}

void D3D12Renderer::ConvertAssetToMyFormat(std::wstring FileName)
{
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName);

	// Temp : ".fbx" ����
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();

	assimpConverter->ExportModelData(FileName);
	assimpConverter->ExportMaterialData(FileName);
}

void D3D12Renderer::ConvertAnimationAssetToMyFormat(std::wstring FileName)
{
	std::shared_ptr<AssimpConverter> assimpConverter = std::make_shared<AssimpConverter>();
	assimpConverter->SetAssetPath(m_assetPath);
	assimpConverter->SetModelPath(m_modelPath);
	assimpConverter->SetTexturePath(m_texturePath);

	assimpConverter->ReadAssetFile(FileName);

	// Temp : ".fbx" ����
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();
	FileName.pop_back();

	assimpConverter->ExportAnimationData(FileName);
}