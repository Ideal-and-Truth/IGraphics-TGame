#include "ParticleSystemManager.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/Resource/Particle/ParticleSystem.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"

Ideal::ParticleSystemManager::ParticleSystemManager()
{

}

Ideal::ParticleSystemManager::~ParticleSystemManager()
{

}

void Ideal::ParticleSystemManager::Init(ComPtr<ID3D12Device> Device, std::shared_ptr<D3D12Shader> Shader)
{
	CreateRootSignature(Device);
	SetVS(Shader);
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> Ideal::ParticleSystemManager::GetRootSignature()
{
	return m_rootSignature;
}

std::shared_ptr<Ideal::D3D12Shader> Ideal::ParticleSystemManager::GetVS()
{
	return m_VS;
}

void Ideal::ParticleSystemManager::CreateRootSignature(ComPtr<ID3D12Device> Device)
{
	//CD3DX12_DESCRIPTOR_RANGE1 ranges[Ideal::ParticleSystemRootSignature::Slot::Count];
	CD3DX12_DESCRIPTOR_RANGE1 ranges[Ideal::ParticleSystemRootSignature::Slot::Count];
	ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Global].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Transform].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
	ranges[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	ranges[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	ranges[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	
	CD3DX12_ROOT_PARAMETER1	rootParameters[Ideal::ParticleSystemRootSignature::Slot::Count];
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::CBV_Global].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Global]);
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::CBV_Transform].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Transform]);
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData]);
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture0].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture0]);
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture1].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture1]);
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture2].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::SRV_ParticleTexture2]);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
	{
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC),
		CD3DX12_STATIC_SAMPLER_DESC(1, D3D12_FILTER_ANISOTROPIC
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		)
	};

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2, staticSamplers, rootSignatureFlags);
	HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	Check(hr, L"Failed to serialize root signature in particle system manager");

	hr = Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf()));
	Check(hr, L"Failed to create RootSignature in particle system manager");

	m_rootSignature->SetName(L"ParticleSystemRootSignature");
}

void Ideal::ParticleSystemManager::SetVS(std::shared_ptr<Ideal::D3D12Shader> Shader)
{
	m_VS = Shader;
}

void Ideal::ParticleSystemManager::AddParticleSystem(std::shared_ptr<Ideal::ParticleSystem> ParticleSystem)
{
	m_particles.push_back(ParticleSystem);
}

void Ideal::ParticleSystemManager::DeleteParticleSystem(std::shared_ptr<Ideal::ParticleSystem>& ParticleSystem)
{
	auto it = std::find_if(m_particles.begin(), m_particles.end(), [&ParticleSystem](const std::shared_ptr<Ideal::ParticleSystem>& p) {return p == ParticleSystem; });
	if (it != m_particles.end()) 
	{
		std::swap(*it, m_particles.back());
		m_particles.pop_back();
	}
}

void Ideal::ParticleSystemManager::DrawParticles(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, CB_Global* CB_GlobalData)
{

	CommandList->SetGraphicsRootSignature(m_rootSignature.Get());
	/// Bind To Shader
	{
		// Global Data 
		auto cb0 = CBPool->Allocate(Device.Get(), sizeof(CB_Global));
		memcpy(cb0->SystemMemAddr, CB_GlobalData, sizeof(CB_Global));
		auto handle0 = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), cb0->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::ParticleSystemRootSignature::Slot::CBV_Global, handle0.GetGpuHandle());
	}

	for (auto& p : m_particles)
	{
		// TODO: DRAW
		if (p->GetActive())
		{
			p->DrawParticle(Device, CommandList, DescriptorHeap, CBPool);
		}
	}
}