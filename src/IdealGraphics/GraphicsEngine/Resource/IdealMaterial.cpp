#include "Core/Core.h"
#include "IdealMaterial.h"
//#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include <d3d12.h>
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/public/IdealRenderer.h"

#include "GraphicsEngine/D3D12/Raytracing/RayTracingFlagManger.h"

#include "ITexture.h"

Ideal::IdealMaterial::IdealMaterial()
{
	m_cbMaterialInfo.bUseDiffuseMap = false;
	m_cbMaterialInfo.bUseNormalMap = false;
	m_cbMaterialInfo.bUseRoughnessMap = false;
	m_cbMaterialInfo.bUseMetallicMap = false;
}

Ideal::IdealMaterial::~IdealMaterial()
{
	// 2024.07.09 필요없어보임
	//m_diffuseTexture.reset();
	//m_specularTexture.reset();
	//m_emissiveTexture.reset();
	//m_normalTexture.reset();
}

void Ideal::IdealMaterial::SetBaseMap(std::shared_ptr<Ideal::ITexture> Texture)
{
	Ideal::Singleton::RayTracingFlagManger::GetInstance().SetTextureChanged();
	// 만약 기존 텍스쳐가 있을경우 Free 해준다.
	m_diffuseTexture = std::static_pointer_cast<Ideal::D3D12Texture>(Texture);
	m_isTextureChanged = true;
}

void Ideal::IdealMaterial::SetNormalMap(std::shared_ptr<Ideal::ITexture> Texture)
{
	Ideal::Singleton::RayTracingFlagManger::GetInstance().SetTextureChanged();
	m_normalTexture = std::static_pointer_cast<Ideal::D3D12Texture>(Texture);
	m_isTextureChanged = true;
}

void Ideal::IdealMaterial::SetMaskMap(std::shared_ptr<Ideal::ITexture> Texture)
{
	Ideal::Singleton::RayTracingFlagManger::GetInstance().SetTextureChanged();
	m_maskTexture = std::static_pointer_cast<Ideal::D3D12Texture>(Texture);
	m_isTextureChanged = true;
}

void Ideal::IdealMaterial::Create(std::shared_ptr<Ideal::ResourceManager> ResourceManager)
{
	//std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(ResourceManager);

	if (m_diffuseTextureFile.find(L".tga") == std::string::npos)
	{
		if (m_diffuseTextureFile.length() > 0)
		{
			m_diffuseTexture = std::make_shared<Ideal::D3D12Texture>();
			ResourceManager->CreateTexture(m_diffuseTexture, m_diffuseTextureFile);
		}
	}

	if (m_specularTextureFile.length() > 0)
	{
		m_specularTexture = std::make_shared<Ideal::D3D12Texture>();
		ResourceManager->CreateTexture(m_specularTexture, m_specularTextureFile);
	}

	if (m_normalTextureFile.length() > 0)
	{
		m_normalTexture = std::make_shared<Ideal::D3D12Texture>();
		ResourceManager->CreateTexture(m_normalTexture, m_normalTextureFile);
	}

	if (m_metallicTextureFile.length() > 0)
	{
		m_metalicTexture = std::make_shared<Ideal::D3D12Texture>();
		ResourceManager->CreateTexture(m_metalicTexture, m_metallicTextureFile);
	}

	if (m_roughnessTextureFile.length() > 0)
	{
		m_roughnessTexture = std::make_shared<Ideal::D3D12Texture>();
		ResourceManager->CreateTexture(m_roughnessTexture, m_roughnessTextureFile);
	}

	m_cbMaterialInfo.metallicFactor = m_metallicFactor;
	m_cbMaterialInfo.roughnessFactor = m_roughnessFactor;
}

void Ideal::IdealMaterial::BindToShader(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();
	auto cb = d3d12Renderer->ConstantBufferAllocate(sizeof(CB_Material));
	if (!cb)
	{
 		__debugbreak();
	}

	// 2024.05.07
	CB_Material* materialData = (CB_Material*)cb->SystemMemAddr;
	materialData->Ambient = m_ambient;
	materialData->Diffuse = m_diffuse;
	materialData->Specular = m_specular;
	materialData->Emissive = m_emissive;

	
	auto handle = descriptorHeap->Allocate(4);	// b1 : Material, t0 : diffuse, t1 : specular, t2 : normal
	uint32 incrementSize = d3d12Renderer->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// TODO : 1번째로 하드코딩 되어 있음
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_CBV_MATERIAL, incrementSize);
	// Root Table Index 1 : 개별 매쉬가 스왑할 root table의 인덱스
	device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// 2024.05.08
	// TEST : 루트시그니쳐에 맞게 두번째로 바꿔준다.
	// 매쉬마다 개별로 쓰는 Root Parameter인 Transform은 Root Paramter Index가 1번이다.
	// 0번은 Transform으로 쓰고 있으니 매쉬마다 갈아 끼워서 써야한다!!
	commandList->SetGraphicsRootDescriptorTable(STATIC_MESH_DESCRIPTOR_TABLE_INDEX_MESH, handle.GetGpuHandle());

	// 2024.05.03 : 그냥 다음 srv인 specular와 normal까지 가져올 것 같은데?
	if (m_diffuseTexture)
	{
		Ideal::D3D12DescriptorHandle diffuseHandle = m_diffuseTexture->GetSRV();
		D3D12_CPU_DESCRIPTOR_HANDLE diffuseCPUAddress = diffuseHandle.GetCpuHandle();

		//auto handle = descriptorHeap->Allocate(1);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_SRV_DIFFUSE, incrementSize);
		device->CopyDescriptorsSimple(1, srvDest, diffuseHandle.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (m_specularTexture)
	{
		Ideal::D3D12DescriptorHandle specularHandle = m_specularTexture->GetSRV();
		D3D12_CPU_DESCRIPTOR_HANDLE specularCPUAddress = specularHandle.GetCpuHandle();

		//auto handle = descriptorHeap->Allocate(1);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_SRV_SPECULAR, incrementSize);
		device->CopyDescriptorsSimple(1, srvDest, specularCPUAddress, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (m_normalTexture)
	{
		Ideal::D3D12DescriptorHandle normalHandle = m_normalTexture->GetSRV();
		D3D12_CPU_DESCRIPTOR_HANDLE normalCPUAddress = normalHandle.GetCpuHandle();
		

		//auto handle = descriptorHeap->Allocate(1);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_SRV_NORMAL, incrementSize);
		device->CopyDescriptorsSimple(1, srvDest, normalCPUAddress, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}

void Ideal::IdealMaterial::Free()
{
	//m_refCountInRayTracing--;
	//if (m_refCountInRayTracing <= 0)
	{
		m_refCountInRayTracing = 0;
		m_diffuseTextureInRayTracing.Free();
		m_normalTextureInRayTracing.Free();
		m_maskTextureInRayTracing.Free();
	}
}

void Ideal::IdealMaterial::CopyHandleToRayTracingDescriptorTable(ComPtr<ID3D12Device> Device)
{
	m_isTextureChanged = false;

	if(m_diffuseTexture)
		Device->CopyDescriptorsSimple(1, m_diffuseTextureInRayTracing.GetCpuHandle(), m_diffuseTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if(m_normalTexture)
		Device->CopyDescriptorsSimple(1, m_normalTextureInRayTracing.GetCpuHandle(), m_normalTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if(m_roughnessTexture)
		Device->CopyDescriptorsSimple(1, m_maskTextureInRayTracing.GetCpuHandle(), m_roughnessTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
