#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "ThirdParty/Include/DirectXTK12/WICTextureLoader.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"

Ideal::D3D12Texture::D3D12Texture()
{

}

Ideal::D3D12Texture::~D3D12Texture()
{

}

void Ideal::D3D12Texture::Create(ComPtr<ID3D12Resource> Resource)
{
	if (m_resource)
	{
		//m_resource->Release();
		m_resource = nullptr;
	}
	m_resource = Resource;
}

void Ideal::D3D12Texture::EmplaceSRV(Ideal::D3D12DescriptorHandle SRVHandle)
{
	m_srvHandle = SRVHandle;
}

void Ideal::D3D12Texture::EmplaceRTV(Ideal::D3D12DescriptorHandle RTVHandle)
{
	m_rtvHandle = RTVHandle;
}

void Ideal::D3D12Texture::EmplaceDSV(Ideal::D3D12DescriptorHandle DSVHandle)
{
	m_dsvHandle = DSVHandle;
}

Ideal::D3D12DescriptorHandle Ideal::D3D12Texture::GetSRV()
{
	if(m_srvHandle.GetCpuHandle().ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		__debugbreak();
	}
	return m_srvHandle;
}

Ideal::D3D12DescriptorHandle Ideal::D3D12Texture::GetRTV()
{
	if (m_rtvHandle.GetCpuHandle().ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		__debugbreak();
	}
	return m_rtvHandle;
}

Ideal::D3D12DescriptorHandle Ideal::D3D12Texture::GetDSV()
{
	if (m_dsvHandle.GetCpuHandle().ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		__debugbreak();
	}
	return m_dsvHandle;
}

