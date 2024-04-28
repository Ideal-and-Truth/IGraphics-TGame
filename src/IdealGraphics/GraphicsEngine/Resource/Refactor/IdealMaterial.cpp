#include "IdealMaterial.h"
#include "GraphicsEngine/public/IdealRenderer.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12ResourceManager.h"
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
		//m_diffuseTexture
	}
}

void Ideal::IdealMaterial::BindToShader(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);

	if (m_diffuseTexture)
	{
		Ideal::D3D12DescriptorHandle diffuseHandle = m_diffuseTexture->GetDescriptorHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE diffuseGPUAddress = diffuseHandle.GetGpuHandle();

		// 2024.04.21 : diffuse texture은 Shader에서 t0라고 가정하고 사용한다.
		// 그리고 이미 Renderer의 SRV의 Heap을 SetDescriptorHeap을 해주었다고 가정한다.
		d3d12Renderer->GetCommandList()->SetGraphicsRootDescriptorTable(STATIC_MESH_ROOT_DESCRIPTOR_TABLE_PIXEL_SRV_INDEX, diffuseGPUAddress);
	}
}
