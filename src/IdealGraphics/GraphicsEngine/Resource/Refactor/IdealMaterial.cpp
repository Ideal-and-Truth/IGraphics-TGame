#include "IdealMaterial.h"
#include "GraphicsEngine/public/IdealRenderer.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

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

	// 2024.05.03 : 그냥 Renderer에 있는 하나의 ConstantBuffer를 번갈아가며 쓴다.
	CB_Material* materialData = (CB_Material*)m_cbMaterial.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	materialData->Ambient	= m_ambient;
	materialData->Diffuse	= m_diffuse;
	materialData->Specular	= m_specular;
	materialData->Emissive	= m_emissive;
	d3d12Renderer->GetCommandList()->SetGraphicsRootConstantBufferView(STATIC_MESH_ROOT_CONSTANT_INDEX_MATERIAL, m_cbMaterial.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));

	// 2024.05.03 : 그냥 다음 srv인 specular와 normal까지 가져올 것 같은데?
	if (m_diffuseTexture)
	{
		Ideal::D3D12DescriptorHandle diffuseHandle = m_diffuseTexture->GetDescriptorHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE diffuseGPUAddress = diffuseHandle.GetGpuHandle();

		// 2024.04.21 : diffuse texture은 Shader에서 t0라고 가정하고 사용한다.
		// 그리고 이미 Renderer의 SRV의 Heap을 SetDescriptorHeap을 해주었다고 가정한다.
		d3d12Renderer->GetCommandList()->SetGraphicsRootDescriptorTable(STATIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SRV_INDEX_DIFFUSE, diffuseGPUAddress);
	}
	if (m_specularTexture)
	{
		Ideal::D3D12DescriptorHandle specularHandle = m_specularTexture->GetDescriptorHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE specularGPUAddress = specularHandle.GetGpuHandle();

		d3d12Renderer->GetCommandList()->SetGraphicsRootDescriptorTable(STATIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SRV_INDEX_SPECULAR, specularGPUAddress);
	}
	if (m_normalTexture)
	{
		Ideal::D3D12DescriptorHandle normalHandle = m_normalTexture->GetDescriptorHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE normalGPUAddress = normalHandle.GetGpuHandle();

		d3d12Renderer->GetCommandList()->SetGraphicsRootDescriptorTable(STATIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SRV_INDEX_NORMAL, normalGPUAddress);
	}
}
