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

void Ideal::ParticleSystem::CreatePipelineState(ComPtr<ID3D12Device> Device)
{
	// TODO : Shader Binding

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { ParticleVertexTest::InputElements, ParticleVertexTest::InputElementCount };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = m_vs->GetShaderByteCode();
	//psoDesc.PS = m_particleMaterial.lock()->GetShader()->GetShaderByteCode();
	psoDesc.PS = m_ps->GetShaderByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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

void Ideal::ParticleSystem::SetRendererMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode)
{
	m_Renderer_Mode = ParticleRendererMode;
}

void Ideal::ParticleSystem::SetRendererMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh)
{
	// TODO : 예외 처리 Particle Vertex가 아닐 경우
	m_Renderer_Mesh = std::static_pointer_cast<Ideal::IdealMesh<ParticleVertexTest>>(ParticleRendererMesh);
}

void Ideal::ParticleSystem::SetRendererMaterial(std::shared_ptr<Ideal::ParticleMaterial> ParticleRendererMaterial)
{
	m_particleMaterial = ParticleRendererMaterial;
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

