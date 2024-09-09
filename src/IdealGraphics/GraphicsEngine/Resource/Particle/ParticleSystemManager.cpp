#include "ParticleSystemManager.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include <d3d12.h>
#include <d3dx12.h>

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
	CD3DX12_DESCRIPTOR_RANGE1 ranges[Ideal::ParticleSystemRootSignature::Slot::Count];
	ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Global].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Transform].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);

	CD3DX12_ROOT_PARAMETER1	rootParameters[Ideal::ParticleSystemRootSignature::Slot::Count];
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::CBV_Global].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Global]);
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::CBV_Transform].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_Transform]);
	rootParameters[Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData].InitAsDescriptorTable(1, &ranges[Ideal::ParticleSystemRootSignature::Slot::CBV_ParticleSystemData]);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
	{
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC)
	};

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, staticSamplers, rootSignatureFlags);
	HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	Check(hr, L"Failed to serialize root signature in particle system manager");

	hr = Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	Check(hr, L"Failed to create RootSignature in particle system manager");

	m_rootSignature->SetName(L"ParticleSystemRootSignature");
}

void Ideal::ParticleSystemManager::SetVS(std::shared_ptr<Ideal::D3D12Shader> Shader)
{
	m_VS = Shader;
}

void Ideal::ParticleSystemManager::AddParticleSystem(std::shared_ptr<Ideal::ParticleSystem> ParticleSystem)
{
	
}

void Ideal::ParticleSystemManager::DeleteParticleSystem(std::shared_ptr<Ideal::ParticleSystem> ParticleSystem)
{

}
