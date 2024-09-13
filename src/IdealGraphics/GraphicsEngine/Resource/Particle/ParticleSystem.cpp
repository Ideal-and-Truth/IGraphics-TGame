#include "GraphicsEngine/Resource/Particle/ParticleSystem.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "IMesh.h"
#include "Misc/Utils/BitUtils.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/Resource/Particle/ParticleMaterial.h"
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"


Ideal::ParticleSystem::ParticleSystem()
{

}

Ideal::ParticleSystem::~ParticleSystem()
{

}

void Ideal::ParticleSystem::SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial)
{
	__debugbreak();
}

void Ideal::ParticleSystem::Init(ComPtr<ID3D12Device> Device, ComPtr<ID3D12RootSignature> RootSignature, std::shared_ptr<Ideal::D3D12Shader> Shader, std::shared_ptr<Ideal::ParticleMaterial> ParticleMaterial)
{
	m_particleMaterial = ParticleMaterial;
	m_rootSignature = RootSignature;
	m_vs = Shader;
	m_ps = m_particleMaterial.lock()->GetShader();
	CreatePipelineState(Device);
}

void Ideal::ParticleSystem::DrawParticle(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool)
{
	// Transform Data
	{
		m_cbTransform.World = Matrix::Identity;
		m_cbTransform.WorldInvTranspose = Matrix::Identity;

		auto cb1 = CBPool->Allocate(Device.Get(), sizeof(CB_Transform));
		memcpy(cb1->SystemMemAddr, &m_cbTransform, sizeof(CB_Transform));
		auto handle1 = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle1.GetCpuHandle(), cb1->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::CBV_Transform, handle1.GetGpuHandle());
	}

	switch (m_Renderer_Mode)
	{
		case Ideal::ParticleMenu::ERendererMode::Billboard:
			break;
		case Ideal::ParticleMenu::ERendererMode::Mesh:
			DrawRenderMesh(Device, CommandList, DescriptorHeap, CBPool);
			break;
		default:
			break;

	}
}

void Ideal::ParticleSystem::CreatePipelineState(ComPtr<ID3D12Device> Device)
{

	//std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	//vs->CompileFromFile(L"../Shaders/Particle/TestCustomParticle2.hlsl", nullptr, nullptr, "VSMain", "vs_5_0");
	//// TODO : shader
	//std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	//ps->CompileFromFile(L"../Shaders/Particle/TestCustomParticle2.hlsl", nullptr, nullptr, "PSMain", "ps_5_0");


	//m_pso = std::make_shared<Ideal::D3D12PipelineStateObject>();
	//// Root Signature
	//m_pso->SetRootSignature(m_rootSignature.Get());
	//// Set IA
	//m_pso->SetInputLayout(ParticleVertexTest::InputElements, ParticleVertexTest::InputElementCount);
	//// VS
	//m_pso->SetVertexShader(m_vs);
	//// PS
	//m_pso->SetPixelShader(m_ps);
	//// RS
	//m_pso->SetRasterizerState();
	//// BS
	//m_pso->SetBlendState();
	//
	//// RTV
	//DXGI_FORMAT rtvFormat[1] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	//// DSV 
	//DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
	//m_pso->SetTargetFormat(1, rtvFormat, dsvFormat);
	//
	//m_pso->Create(Device.Get());
	//return;

	//std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	//vs->CompileFromFile(L"../Shaders/Particle/TestCustomParticle2.hlsl", nullptr, nullptr, "VSMain", "vs_5_0");
	//// TODO : shader
	//std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	//ps->CompileFromFile(L"../Shaders/Particle/TestCustomParticle2.hlsl", nullptr, nullptr, "PSMain", "ps_5_0");


	// TODO : Shader Binding

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { ParticleVertexTest::InputElements, ParticleVertexTest::InputElementCount };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = m_vs->GetShaderByteCode();
	//psoDesc.PS = m_particleMaterial.lock()->GetShader()->GetShaderByteCode();
	psoDesc.PS = m_ps->GetShaderByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	{
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;	// one // zero 일경우 검은색으로 바뀌어간다.
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		psoDesc.BlendState = blendDesc;
	}
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;

	HRESULT hr = Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pipelineState.GetAddressOf()));
	Check(hr, L"Faild to Create Pipeline State");
	return;
}

void Ideal::ParticleSystem::SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode)
{
	m_Renderer_Mode = ParticleRendererMode;
}

void Ideal::ParticleSystem::SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh)
{
	// TODO : 예외 처리 Particle Vertex가 아닐 경우
	m_Renderer_Mesh = std::static_pointer_cast<Ideal::IdealMesh<ParticleVertexTest>>(ParticleRendererMesh);
}

void Ideal::ParticleSystem::SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial)
{
	m_particleMaterial = std::static_pointer_cast<Ideal::ParticleMaterial>(ParticleRendererMaterial);
}

void Ideal::ParticleSystem::SetCustomData(Ideal::ParticleMenu::ECustomData CustomData, Ideal::ParticleMenu::ECustomDataParameter CustomDataParameter, Ideal::ParticleMenu::ERangeMode RangeMode, float CustomDataFloat, float CustomDataFloat2 /*= 0.f*/)
{
	float* Data = nullptr;
	if (CustomData == Ideal::ParticleMenu::ECustomData::CustomData_1)
	{
		Data = m_cbParticleSystem.CustomData1;
	}
	else
	{
		Data = m_cbParticleSystem.CustomData2;
	}

	uint32 parameter = static_cast<uint32>(CustomDataParameter);

	switch (RangeMode)
	{
		case Ideal::ParticleMenu::ERangeMode::Constant:
		{
			Data[parameter] = CustomDataFloat;
		}
		break;
		case Ideal::ParticleMenu::ERangeMode::Curve:
		{
			Data[parameter] = CustomDataFloat;
			// TODO: CURVE
		}
		break;
		case Ideal::ParticleMenu::ERangeMode::RandomBetweenConstants:
		{
			__debugbreak();
		}
			break;
		case Ideal::ParticleMenu::ERangeMode::RandomBetweenCurves:
		{
			__debugbreak();
		}
			break;
		default:
			break;

	}
}

void Ideal::ParticleSystem::DrawRenderMesh(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool)
{
	//Draw Mesh
	//CommandList->SetPipelineState(m_pso->GetPipelineState().Get());
	CommandList->SetPipelineState(m_pipelineState.Get());
	// 
	//CommandList->SetGraphicsRootSignature(m_rootSignature.Get());	// 이거 위에서 해도 되지 않을까? 위에서 하면 루트 시그니쳐는 같으니까 한 번만 해도 되지 않을까?
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = m_Renderer_Mesh.lock()->GetVertexBufferView();
	const D3D12_INDEX_BUFFER_VIEW& indexBufferView = m_Renderer_Mesh.lock()->GetIndexBufferView();

	CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	CommandList->IASetIndexBuffer(&indexBufferView);

	// CB_ParticleSystem
	{
		// TEMP: time
		m_cbParticleSystem.Time += 0.001f;
		if (m_cbParticleSystem.Time >= 1.f) m_cbParticleSystem.Time = 0.f;

		auto cb2 = CBPool->Allocate(Device.Get(), sizeof(CB_ParticleSystem));
		memcpy(cb2->SystemMemAddr, &m_cbParticleSystem, sizeof(CB_ParticleSystem));
		auto handle2 = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb2->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData, handle2.GetGpuHandle());
	}
	// SRV
	if (m_particleMaterial.lock())
	{
		auto texture = m_particleMaterial.lock()->GetTexture().lock();
		if (texture)
		{
			auto handle3 = DescriptorHeap->Allocate();
			Device->CopyDescriptorsSimple(1, handle3.GetCpuHandle(), texture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture, handle3.GetGpuHandle());
		}
	}
	CommandList->DrawIndexedInstanced(m_Renderer_Mesh.lock()->GetElementCount(), 1, 0, 0, 0);
}
