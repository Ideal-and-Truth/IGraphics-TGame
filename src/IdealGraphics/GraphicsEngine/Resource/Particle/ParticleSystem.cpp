#include "GraphicsEngine/Resource/Particle/ParticleSystem.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "IMesh.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/Resource/Particle/ParticleMaterial.h"
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"


Ideal::ParticleSystem::ParticleSystem()
{
	m_cbTransform.World = Matrix::Identity;
	m_cbTransform.WorldInvTranspose = Matrix::Identity;
}

Ideal::ParticleSystem::~ParticleSystem()
{

}

void Ideal::ParticleSystem::SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial)
{
	__debugbreak();
}

std::weak_ptr<Ideal::IParticleMaterial> Ideal::ParticleSystem::GetMaterial()
{
	return m_particleMaterial;
}

void Ideal::ParticleSystem::SetTransformMatrix(const Matrix& Transform)
{
	m_transform = Transform;
}

const DirectX::SimpleMath::Matrix& Ideal::ParticleSystem::GetTransformMatrix() const
{
	return m_transform;
}

void Ideal::ParticleSystem::SetActive(bool IsActive)
{
	m_isActive = IsActive;
	if (m_playOnWake)
	{
		Play();
	}
}

bool Ideal::ParticleSystem::GetActive()
{
	return m_isActive;
}

void Ideal::ParticleSystem::SetDeltaTime(float DT)
{
	m_deltaTime = DT;
}

void Ideal::ParticleSystem::Play()
{
	m_isPlaying = true;
	m_currentTime = 0;
	m_currentDurationTime = 0;
}

void Ideal::ParticleSystem::Pause()
{
	m_isPlaying = false;
}

void Ideal::ParticleSystem::SetStopWhenFinished(bool StopWhenFinished)
{
	m_stopWhenFinished = StopWhenFinished;
}

void Ideal::ParticleSystem::SetPlayOnWake(bool PlayOnWake)
{
	m_playOnWake = PlayOnWake;
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
	m_currentTime += m_deltaTime;
	if (m_isPlaying)
	{
		m_currentDurationTime += (m_deltaTime * m_simulationSpeed);
	}

	if (m_currentDurationTime >= m_duration)
	{
		if (!m_isLoop || m_stopWhenFinished)
		{
			// 반복이 아닐때 그냥 나간다.
			// 또는 한 번만 재생을 원하면 나간다.
			return;
		}
		m_currentDurationTime = 0.f;
	}

	if (m_currentDurationTime > m_startLifetime)
	{
		return;
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
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { ParticleVertexTest::InputElements, ParticleVertexTest::InputElementCount };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = m_vs->GetShaderByteCode();
	psoDesc.PS = m_ps->GetShaderByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	{
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		// --- blending mode ---//
		Ideal::ParticleMaterialMenu::EBlendingMode BlendMode = m_particleMaterial.lock()->GetBlendingMode();
		switch (BlendMode)
		{
			case Ideal::ParticleMaterialMenu::EBlendingMode::Additive:
			{
				blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;	// one // zero 일경우 검은색으로 바뀌어간다.
			}
				break;
			case Ideal::ParticleMaterialMenu::EBlendingMode::Alpha:
			{
				blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;	// one // zero 일경우 검은색으로 바뀌어간다.
			}
				break;
			default:
				break;
		}
		

	
		//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;	// one // zero 일경우 검은색으로 바뀌어간다.
		//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;	// one // zero 일경우 검은색으로 바뀌어간다.
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		psoDesc.BlendState = blendDesc;
	}
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
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

void Ideal::ParticleSystem::SetStartColor(const DirectX::SimpleMath::Color& StartColor)
{
	m_startColor = StartColor;
}

DirectX::SimpleMath::Color& Ideal::ParticleSystem::GetStartColor()
{
	//return m_cbParticleSystem.StartColor;
	return m_startColor;
}

void Ideal::ParticleSystem::SetStartLifetime(float Time)
{
	m_startLifetime = Time;
}

float Ideal::ParticleSystem::GetStartLifetime()
{
	return m_startLifetime;
}

void Ideal::ParticleSystem::SetSimulationSpeed(float Speed)
{
	m_simulationSpeed = Speed;
}

float Ideal::ParticleSystem::GetSimulationSpeed()
{
	return m_simulationSpeed;
}

void Ideal::ParticleSystem::SetDuration(float Time)
{
	m_duration = Time;
}

float Ideal::ParticleSystem::GetDuration()
{
	return m_duration;
}

void Ideal::ParticleSystem::SetLoop(bool Loop)
{
	m_isLoop = Loop;
}

bool Ideal::ParticleSystem::GetLoop()
{
	return m_isLoop;
}

void Ideal::ParticleSystem::SetColorOverLifetime(bool Active)
{
	m_isUseColorOverLifetime = Active;
}

Ideal::IGradient& Ideal::ParticleSystem::GetColorOverLifetimeGradientGraph()
{
	return m_ColorOverLifetimeGradientGraph;
}

void Ideal::ParticleSystem::UpdateColorOverLifetime()
{
	if (m_isUseColorOverLifetime)
	{
		// 현재 그래프의 색상을 불러온다.
		Color colorAtCurrentTime = m_ColorOverLifetimeGradientGraph.GetColorAtPosition(m_currentDurationTime / m_duration);
		m_cbParticleSystem.StartColor = colorAtCurrentTime;
		//Color colorAtCurrentTime = m_ColorOverLifetimeGradientGraph.GetColorAtPosition(m_currentTime);
		//if (m_currentTime > 0.8f)
		//{
		//	int a = 3;
		//}
		//if (m_currentTime > 1.f)
		//{
		//	m_cbParticleSystem.StartColor.A(0.f);
		//}
	}
	else
	{
		m_cbParticleSystem.StartColor = m_startColor;
	}
}

void Ideal::ParticleSystem::SetRotationOverLifetime(bool Active)
{
	m_isRotationOverLifetime = Active;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetRotationOverLifetimeAxisX()
{
	return m_RotationOverLifetimeAxisX;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetRotationOverLifetimeAxisY()
{
	return m_RotationOverLifetimeAxisY;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetRotationOverLifetimeAxisZ()
{
	return m_RotationOverLifetimeAxisZ;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData1X()
{
	return m_CustomData1_X;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData1Y()
{
	return m_CustomData1_Y;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData1Z()
{
	return m_CustomData1_Z;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData1W()
{
	return m_CustomData1_W;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData2X()
{
	return m_CustomData2_X;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData2Y()
{
	return m_CustomData2_Y;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData2Z()
{
	return m_CustomData2_Z;
}

Ideal::IBezierCurve& Ideal::ParticleSystem::GetCustomData2W()
{
	return m_CustomData2_W;
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
	
	// Transform Data
	{
		// RotationOverLifetime
		Matrix cal = Matrix::Identity;
		if (m_isRotationOverLifetime)
		{
			float xRot = m_RotationOverLifetimeAxisX.GetPoint(m_currentDurationTime).y;
			auto matX = Matrix::CreateRotationX(xRot * 3.14);
			float yRot = m_RotationOverLifetimeAxisY.GetPoint(m_currentDurationTime).y;
			auto matY = Matrix::CreateRotationY(yRot * 3.14);
			float zRot = m_RotationOverLifetimeAxisZ.GetPoint(m_currentDurationTime).y;
			auto matZ = Matrix::CreateRotationZ(zRot * 3.14);
			cal *= matX;
			cal *= matY;
			cal *= matZ;
			m_cbTransform.World = cal.Transpose();
		}
		m_cbTransform.World *= m_transform;
		m_cbTransform.World = m_cbTransform.World.Transpose();
		m_cbTransform.WorldInvTranspose = m_cbTransform.World.Invert();

		auto cb1 = CBPool->Allocate(Device.Get(), sizeof(CB_Transform));
		memcpy(cb1->SystemMemAddr, &m_cbTransform, sizeof(CB_Transform));
		auto handle1 = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle1.GetCpuHandle(), cb1->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::CBV_Transform, handle1.GetGpuHandle());
	}

	//Draw Mesh
	CommandList->SetPipelineState(m_pipelineState.Get());
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = m_Renderer_Mesh.lock()->GetVertexBufferView();
	const D3D12_INDEX_BUFFER_VIEW& indexBufferView = m_Renderer_Mesh.lock()->GetIndexBufferView();

	CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	CommandList->IASetIndexBuffer(&indexBufferView);

	// CB_ParticleSystem
	{
		UpdateCustomData();
		UpdateColorOverLifetime();
		//if (m_currentTime > 1.0) m_currentTime = 1.0;
		m_cbParticleSystem.Time = m_currentTime;
		auto cb2 = CBPool->Allocate(Device.Get(), sizeof(CB_ParticleSystem));
		memcpy(cb2->SystemMemAddr, &m_cbParticleSystem, sizeof(CB_ParticleSystem));
		auto handle2 = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb2->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData, handle2.GetGpuHandle());
	}
	// SRV
	if (m_particleMaterial.lock())
	{
		// Texture0
		{
			auto texture = m_particleMaterial.lock()->GetTexture0().lock();
			if (texture)
			{
				auto handle3 = DescriptorHeap->Allocate();
				Device->CopyDescriptorsSimple(1, handle3.GetCpuHandle(), texture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture0, handle3.GetGpuHandle());
			}
		}
		// Texture1
		{
			auto texture = m_particleMaterial.lock()->GetTexture1().lock();
			if (texture)
			{
				auto handle4 = DescriptorHeap->Allocate();
				Device->CopyDescriptorsSimple(1, handle4.GetCpuHandle(), texture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture1, handle4.GetGpuHandle());
			}
		}
		// Texture0
		{
			auto texture = m_particleMaterial.lock()->GetTexture2().lock();
			if (texture)
			{
				auto handle5 = DescriptorHeap->Allocate();
				Device->CopyDescriptorsSimple(1, handle5.GetCpuHandle(), texture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture2, handle5.GetGpuHandle());
			}
		}
	}
	CommandList->DrawIndexedInstanced(m_Renderer_Mesh.lock()->GetElementCount(), 1, 0, 0, 0);
}

void Ideal::ParticleSystem::UpdateCustomData()
{
	{
		float customX = m_CustomData1_X.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData1[0] = customX;
		float customY = m_CustomData1_Y.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData1[1] = customY;
		float customZ = m_CustomData1_Z.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData1[2] = customZ;
		float customW = m_CustomData1_W.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData1[3] = customW;
	}
	{
		float customX = m_CustomData2_X.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData2[0] = customX;
		float customY = m_CustomData2_Y.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData2[1] = customY;
		float customZ = m_CustomData2_Z.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData2[2] = customZ;
		float customW = m_CustomData2_W.GetPoint(m_currentDurationTime).y;
		m_cbParticleSystem.CustomData2[3] = customW;
	}
}

void Ideal::ParticleSystem::UpdateLifeTime()
{
	if (m_currentTime > m_startLifetime)
	{
		
	}
}
