#include "IdealRenderScene.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealScreenQuad.h"

#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12Viewport.h"

#include "GraphicsEngine/ConstantBufferInfo.h"

Ideal::IdealRenderScene::IdealRenderScene()
{

}

Ideal::IdealRenderScene::~IdealRenderScene()
{

}

void Ideal::IdealRenderScene::Init(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);

	m_width = d3d12Renderer->GetWidth();
	m_height = d3d12Renderer->GetHeight();

	//---GBuffer SRV RTV---//
	CreateGBuffer(Renderer);

	//---Screen Quad---//
	InitScreenQuad(Renderer);
	CreateScreenQuadRootSignature(Renderer);
	CreateScreenQuadPSO(Renderer);

	//---Global CB---//
	CreateGlobalCB(Renderer);

	//---Mesh---//
	CreateStaticMeshPSO(Renderer);
	CreateSkinnedMeshPSO(Renderer);

}

void Ideal::IdealRenderScene::Draw(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	
	commandList->SetDescriptorHeaps(1, d3d12Renderer->GetMainDescriptorHeap()->GetDescriptorHeap().GetAddressOf());

	// CB
	UpdateGlobalCBData(Renderer);

	// Ver2 StaticMesh
	{
		commandList->SetPipelineState(m_staticMeshPSO->GetPipelineState().Get());
		commandList->SetGraphicsRootSignature(m_staticMeshRootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		SetGlobalCBDescriptorTable(Renderer);

		for (auto& m : m_staticMeshObjects)
		{
			if (m.lock() != nullptr)
			{
				m.lock()->Draw(d3d12Renderer);
			}
		}
	}
	// Ver2 SkinnedMesh
	{
		commandList->SetPipelineState(m_skinnedMeshPSO->GetPipelineState().Get());
		commandList->SetGraphicsRootSignature(m_skinnedMeshRootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		SetGlobalCBDescriptorTable(Renderer);

		for (auto& m : m_skinnedMeshObjects)
		{
			if (m.lock() != nullptr)
			{
				m.lock()->Draw(Renderer);
			}
		}
	}
}

void Ideal::IdealRenderScene::DrawGBuffer(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	// RenderTarget 설정
	TransitionGBufferToRTVandClear(Renderer);

	commandList->SetDescriptorHeaps(1, d3d12Renderer->GetMainDescriptorHeap()->GetDescriptorHeap().GetAddressOf());

	// CB
	UpdateGlobalCBData(Renderer);

	// Ver2 StaticMesh
	{
		commandList->SetPipelineState(m_staticMeshPSO->GetPipelineState().Get());
		commandList->SetGraphicsRootSignature(m_staticMeshRootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		SetGlobalCBDescriptorTable(Renderer);

		for (auto& m : m_staticMeshObjects)
		{
			if (m.lock() != nullptr)
			{
				m.lock()->Draw(d3d12Renderer);
			}
		}
	}
	// Ver2 SkinnedMesh
	/*{
		commandList->SetPipelineState(m_skinnedMeshPSO->GetPipelineState().Get());
		commandList->SetGraphicsRootSignature(m_skinnedMeshRootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		SetGlobalCBDescriptorTable(Renderer);

		for (auto& m : m_skinnedMeshObjects)
		{
			if (m.lock() != nullptr)
			{
				m.lock()->Draw(Renderer);
			}
		}
	}*/

	/*commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	d3d12Renderer->GetCommandQueue()->ExecuteCommandLists(1, ppCommandLists);
	d3d12Renderer->GraphicsFence();
	d3d12Renderer->WaitForGraphicsFenceValue();
	d3d12Renderer->ResetCommandList();*/
}

void Ideal::IdealRenderScene::DrawScreen(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	commandList->SetPipelineState(m_screenQuadPSO->GetPipelineState().Get());
	commandList->SetGraphicsRootSignature(m_screenQuadRootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SetGlobalCBDescriptorTable(Renderer);

	TransitionGBufferToSRV(Renderer);

	m_fullScreenQuad->Draw(Renderer, m_gBuffers);
}

void Ideal::IdealRenderScene::AddObject(std::shared_ptr<Ideal::IMeshObject> MeshObject)
{
	if (std::dynamic_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject) != nullptr)
	{
		m_staticMeshObjects.push_back(std::static_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject));
	}
	else if (std::dynamic_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject) != nullptr)
	{
		m_skinnedMeshObjects.push_back(std::static_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject));
	}
}

void Ideal::IdealRenderScene::CreateStaticMeshPSO(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);

	//-------------------Sampler--------------------//
	CD3DX12_STATIC_SAMPLER_DESC sampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	//-------------------Range--------------------//
	CD3DX12_DESCRIPTOR_RANGE1 rangeGlobal[1];
	rangeGlobal[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0 : Global

	CD3DX12_DESCRIPTOR_RANGE1 rangePerObj[1];
	rangePerObj[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);	// b1 : Transform

	CD3DX12_DESCRIPTOR_RANGE1 rangePerMesh[2];
	rangePerMesh[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);	// b2 : Material	// mesh마다 다른 material을 가지고 있으니 갈아 끼워야한다.
	rangePerMesh[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);	// t0 ~ t2 : Shader Resource View

	//-------------------Parameter--------------------//
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	rootParameters[0].InitAsDescriptorTable(_countof(rangeGlobal), rangeGlobal, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(_countof(rangePerObj), rangePerObj, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsDescriptorTable(_countof(rangePerMesh), rangePerMesh, D3D12_SHADER_VISIBILITY_ALL);


	//-------------------Signature--------------------//
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Check(d3d12Renderer->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_staticMeshRootSignature.GetAddressOf())
	));

	m_staticMeshPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	m_staticMeshPSO->SetInputLayout(BasicVertex::InputElements, BasicVertex::InputElementCount);

	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	vs->CompileFromFile(L"../Shaders/GBufferMesh.hlsl", nullptr, nullptr, "VS", "vs_5_0");
	m_staticMeshPSO->SetVertexShader(vs);

	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	ps->CompileFromFile(L"../Shaders/GBufferMesh.hlsl", nullptr, nullptr, "PS", "ps_5_0");
	m_staticMeshPSO->SetPixelShader(ps);

	m_staticMeshPSO->SetRootSignature(m_staticMeshRootSignature.Get());
	m_staticMeshPSO->SetRasterizerState();
	m_staticMeshPSO->SetBlendState();
	m_staticMeshPSO->Create(d3d12Renderer);
}

void Ideal::IdealRenderScene::CreateSkinnedMeshPSO(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);

	//-------------------Sampler--------------------//
	CD3DX12_STATIC_SAMPLER_DESC sampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);
	//-------------------Range--------------------//
	CD3DX12_DESCRIPTOR_RANGE1 rangeGlobal[1];
	rangeGlobal[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : global

	CD3DX12_DESCRIPTOR_RANGE1 rangePerObj[1];
	rangePerObj[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 1); // b1 : transform, b1 : bone

	CD3DX12_DESCRIPTOR_RANGE1 rangePerMesh[2];
	rangePerMesh[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 3);	// b3 : Material	// mesh마다 다른 material을 가지고 있으니 갈아 끼워야한다.
	rangePerMesh[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);	// t0 ~ t2 : Shader Resource View


	//-------------------Parameter--------------------//
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	rootParameters[0].InitAsDescriptorTable(_countof(rangeGlobal), rangeGlobal, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(_countof(rangePerObj), rangePerObj, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsDescriptorTable(_countof(rangePerMesh), rangePerMesh, D3D12_SHADER_VISIBILITY_ALL);


	//-------------------Signature--------------------//
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Check(d3d12Renderer->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_skinnedMeshRootSignature.GetAddressOf())
	));

	m_skinnedMeshPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	m_skinnedMeshPSO->SetInputLayout(SkinnedVertex::InputElements, SkinnedVertex::InputElementCount);

	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	vs->CompileFromFile(L"../Shaders/AnimationDefault.hlsl", nullptr, nullptr, "VS", "vs_5_0");
	m_skinnedMeshPSO->SetVertexShader(vs);

	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	ps->CompileFromFile(L"../Shaders/AnimationDefault.hlsl", nullptr, nullptr, "PS", "ps_5_0");
	m_skinnedMeshPSO->SetPixelShader(ps);

	m_skinnedMeshPSO->SetRootSignature(m_skinnedMeshRootSignature.Get());
	m_skinnedMeshPSO->SetRasterizerState();
	m_skinnedMeshPSO->SetBlendState();
	m_skinnedMeshPSO->Create(d3d12Renderer);
}

void Ideal::IdealRenderScene::CreateGlobalCB(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	m_cbGlobal = std::make_shared<CB_Global>();
}

void Ideal::IdealRenderScene::UpdateGlobalCBData(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();

	m_cbGlobal->View = d3d12Renderer->GetView();
	m_cbGlobal->Proj = d3d12Renderer->GetProj();
	m_cbGlobal->ViewProj = d3d12Renderer->GetViewProj();

	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();
	std::shared_ptr<Ideal::D3D12ConstantBufferPool> cbPool = d3d12Renderer->GetCBPool(sizeof(CB_Global));

	auto cb = cbPool->Allocate();
	if (!cb)
	{
		__debugbreak();
	}

	CB_Global* cbGlobal = (CB_Global*)cb->SystemMemAddr;
	//*cbGlobal = *(m_cbGlobal.get());
	*cbGlobal = *m_cbGlobal;

	auto handle = descriptorHeap->Allocate(1);
	uint32 incrementSize = d3d12Renderer->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Copy To Main Descriptor
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), GLOBAL_DESCRIPTOR_INDEX_CBV_GLOBAL, incrementSize);
	device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_cbGlobalHandle = handle;
}

void Ideal::IdealRenderScene::SetGlobalCBDescriptorTable(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	commandList->SetGraphicsRootDescriptorTable(GLOBAL_DESCRIPTOR_TABLE_INDEX, m_cbGlobalHandle.GetGpuHandle());
}

void Ideal::IdealRenderScene::CreateGBuffer(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	std::shared_ptr<Ideal::ResourceManager> resourceManager = d3d12Renderer->GetResourceManager();

	for (uint32 i = 0; i < m_gBufferNum; ++i)
	{
		std::shared_ptr<Ideal::D3D12Texture> gBuffer = nullptr;
		resourceManager->CreateEmptyTexture2D(gBuffer, m_width, m_height, true);
		m_gBuffers.push_back(gBuffer);
	}

	m_gBufferClearColors[0] = DirectX::Colors::AliceBlue;
	m_gBufferClearColors[1] = DirectX::Colors::Brown;
	m_gBufferClearColors[2] = DirectX::Colors::Violet;
	m_gBufferClearColors[3] = DirectX::Colors::Green;
}

void Ideal::IdealRenderScene::TransitionGBufferToRTVandClear(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	std::shared_ptr<Ideal::ResourceManager> resourceManager = d3d12Renderer->GetResourceManager();

	// 2024.05.17 이거 안해주면 rtv에 안그려지네?
	commandList->RSSetViewports(1, &(d3d12Renderer->GetViewport()->GetViewport()));
	commandList->RSSetScissorRects(1, &(d3d12Renderer->GetViewport()->GetScissorRect()));

	for (auto t : m_gBuffers)
	{
		CD3DX12_RESOURCE_BARRIER backBufferRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
			t->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		commandList->ResourceBarrier(1, &backBufferRenderTarget);
	}

	// TODO : OMSetRenderTarget
	//commandList->OMSetRenderTargets(m_gBufferNum, &(m_gBuffers[0]->GetRTV().GetCpuHandle()), FALSE, nullptr);
	//commandList->OMSetRenderTargets(1, &resourceManager->GetRTVHeap(), FALSE, nullptr);
	//commandList->OMSetRenderTargets(4, &(m_gBuffers[0]->GetRTV().GetCpuHandle()), TRUE, nullptr);
	commandList->OMSetRenderTargets(4, &(m_gBuffers[0]->GetRTV().GetCpuHandle()), TRUE, &d3d12Renderer->GetDSV());
	commandList->ClearDepthStencilView(d3d12Renderer->GetDSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	//commandList->OMSetRenderTargets(1, &resourceManager->GetRTVHeap(), FALSE, nullptr);
	

	// TODO : ClearRenderTarget
	for (uint32 i = 0; i < m_gBufferNum; ++i)
	{
		commandList->ClearRenderTargetView(m_gBuffers[i]->GetRTV().GetCpuHandle(), m_gBufferClearColors[i], 0, nullptr);
	}
}

void Ideal::IdealRenderScene::TransitionGBufferToSRV(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	for (auto t : m_gBuffers)
	{
		CD3DX12_RESOURCE_BARRIER backBufferRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
			t->GetResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		commandList->ResourceBarrier(1, &backBufferRenderTarget);
	}
}

void Ideal::IdealRenderScene::InitScreenQuad(std::shared_ptr<IdealRenderer> Renderer)
{
	m_fullScreenQuad = std::make_shared<Ideal::IdealScreenQuad>();
	m_fullScreenQuad->Init(Renderer);
}

void Ideal::IdealRenderScene::CreateScreenQuadRootSignature(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);

	//-------------------Sampler--------------------//
	CD3DX12_STATIC_SAMPLER_DESC sampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);
	//-------------------Range--------------------//
	CD3DX12_DESCRIPTOR_RANGE1 rangeGlobal[1];
	rangeGlobal[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : global

	CD3DX12_DESCRIPTOR_RANGE1 rangeGBuffers[1];
	rangeGBuffers[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0);	// t0 : Albedo, t1 : Normal, t2 : POSH, t3 : Tangent...


	//-------------------Parameter--------------------//
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];
	rootParameters[0].InitAsDescriptorTable(_countof(rangeGlobal), rangeGlobal, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(_countof(rangeGBuffers), rangeGBuffers, D3D12_SHADER_VISIBILITY_ALL);


	//-------------------Signature--------------------//
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Check(d3d12Renderer->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_screenQuadRootSignature.GetAddressOf())
	));
}

void Ideal::IdealRenderScene::CreateScreenQuadPSO(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);

	m_screenQuadPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	m_screenQuadPSO->SetInputLayout(ScreenQuadVertex::InputElements, ScreenQuadVertex::InputElementCount);

	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	vs->CompileFromFile(L"../Shaders/FullScreenVS.hlsl", nullptr, nullptr, "main", "vs_5_0");
	m_screenQuadPSO->SetVertexShader(vs);

	// TODO : Light
	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	ps->CompileFromFile(L"../Shaders/LightPassPS.hlsl", nullptr, nullptr, "main", "ps_5_0");
	m_screenQuadPSO->SetPixelShader(ps);

	m_screenQuadPSO->SetRootSignature(m_screenQuadRootSignature.Get());
	m_screenQuadPSO->SetRasterizerState();
	m_screenQuadPSO->SetBlendState();
	m_screenQuadPSO->Create(d3d12Renderer);
}
