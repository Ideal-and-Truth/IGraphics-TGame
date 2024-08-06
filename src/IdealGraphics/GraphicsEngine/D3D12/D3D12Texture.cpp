#include "Core/Core.h"
//#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "ThirdParty/Include/DirectXTK12/WICTextureLoader.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/DeferredDeleteManager.h"
#include <d3d12.h>
#include <d3dx12.h>

Ideal::D3D12Texture::D3D12Texture()
{

}

Ideal::D3D12Texture::~D3D12Texture()
{
	m_srvHandle.Free();
	m_rtvHandle.Free();
	m_dsvHandle.Free();

	m_srvHandleInEditor.Free();
}

uint64 Ideal::D3D12Texture::GetImageID()
{
	return static_cast<uint64>(m_srvHandleInEditor.GetGpuHandle().ptr);
}

uint32 Ideal::D3D12Texture::GetWidth()
{
	uint64 ret = m_resource->GetDesc().Width;
	return (uint32)ret;
}

uint32 Ideal::D3D12Texture::GetHeight()
{
	uint64 ret = m_resource->GetDesc().Height;
	return (uint32)ret;
}

void Ideal::D3D12Texture::Create(ComPtr<ID3D12Resource> Resource, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager)
{
	m_resource = Resource;
	m_deferredDeleteManager = DeferredDeleteManager;
}

void Ideal::D3D12Texture::Free()
{
	//m_refCount--;
	//if (m_refCount <= 0)
	//{
	//	m_refCount = 0;
	//	m_deferredDeleteManager.lock()->AddTextureToDeferredDelete(shared_from_this());
	//}
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

void Ideal::D3D12Texture::EmplaceSRVInEditor(Ideal::D3D12DescriptorHandle SRVHandle)
{
	m_srvHandleInEditor = SRVHandle;
}

