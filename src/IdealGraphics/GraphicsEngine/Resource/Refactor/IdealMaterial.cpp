#include "IdealMaterial.h"
#include "GraphicsEngine/public/IdealRenderer.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"

Ideal::IdealMaterial::IdealMaterial()
{

}

Ideal::IdealMaterial::~IdealMaterial()
{

}

void Ideal::IdealMaterial::Create(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);

	if (m_diffuseTextureFile.length() > 0)
	{
		m_diffuseTexture = std::make_shared<Ideal::D3D12Texture>();
		d3d12Renderer->GetResourceManager()->CreateTexture(m_diffuseTexture, m_diffuseTextureFile);
	}

	if (m_specularTextureFile.length() > 0)
	{
		m_specularTexture = std::make_shared<Ideal::D3D12Texture>();
		d3d12Renderer->GetResourceManager()->CreateTexture(m_specularTexture, m_specularTextureFile);
	}

	if (m_normalTextureFile.length() > 0)
	{
		m_normalTexture = std::make_shared<Ideal::D3D12Texture>();
		d3d12Renderer->GetResourceManager()->CreateTexture(m_normalTexture, m_normalTextureFile);
	}

	/*if (m_specularTextureFile.length() > 0)
	{
		m_specularTexture = std::make_shared<Ideal::D3D12Texture>();
		d3d12Renderer->GetResourceManager()->CreateTexture(m_specularTexture, m_specularTextureFile);
	}*/

	//------------------Material CB-------------------//
	m_cbMaterial.Create(d3d12Renderer->GetDevice().Get(), sizeof(CB_Material), D3D12Renderer::FRAME_BUFFER_COUNT);
}

void Ideal::IdealMaterial::BindToShader(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();
	std::shared_ptr<Ideal::D3D12ConstantBufferPool> cbPool = d3d12Renderer->GetCBPool(sizeof(CB_Material));

	auto cb = cbPool->Allocate();
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
	device->CopyDescriptorsSimple(STATIC_MESH_DESCRIPTOR_TABLE_INDEX_MESH, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// 2024.05.08
	// TEST : 루트시그니쳐에 맞게 두번째로 바꿔준다.
	// 매쉬마다 개별로 쓰는 Root Parameter인 Transform은 Root Paramter Index가 1번이다.
	// 0번은 Transform으로 쓰고 있으니 매쉬마다 갈아 끼워서 써야한다!!
	commandList->SetGraphicsRootDescriptorTable(1, handle.GetGpuHandle());

	// 2024.05.03 : 그냥 다음 srv인 specular와 normal까지 가져올 것 같은데?
	if (m_diffuseTexture)
	{
		Ideal::D3D12DescriptorHandle diffuseHandle = m_diffuseTexture->GetDescriptorHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE diffuseCPUAddress = diffuseHandle.GetCpuHandle();

		//auto handle = descriptorHeap->Allocate(1);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_SRV_DIFFUSE, incrementSize);
		device->CopyDescriptorsSimple(1, srvDest, diffuseHandle.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (m_specularTexture)
	{
		Ideal::D3D12DescriptorHandle specularHandle = m_specularTexture->GetDescriptorHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE specularCPUAddress = specularHandle.GetCpuHandle();

		//auto handle = descriptorHeap->Allocate(1);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_SRV_SPECULAR, incrementSize);
		device->CopyDescriptorsSimple(1, srvDest, specularCPUAddress, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (m_normalTexture)
	{
		Ideal::D3D12DescriptorHandle normalHandle = m_normalTexture->GetDescriptorHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE normalCPUAddress = normalHandle.GetCpuHandle();
		

		//auto handle = descriptorHeap->Allocate(1);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_SRV_NORMAL, incrementSize);
		device->CopyDescriptorsSimple(1, srvDest, normalCPUAddress, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}
