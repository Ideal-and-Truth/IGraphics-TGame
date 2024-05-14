#pragma once
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

struct ID3D12Resource;

namespace Ideal
{
	class D3D12Renderer;
}

namespace Ideal
{
	class D3D12Texture : public D3D12Resource
	{
	public:
		D3D12Texture();
		virtual ~D3D12Texture();

	public:
		// ResourceManager에서 호출된다.
		void Create(
			ComPtr<ID3D12Resource> Resource, Ideal::D3D12DescriptorHandle Handle
		);

		// 2024.04.21 : 디스크립터가 할당된 위치를 가져온다.
		//D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() { return m_srvHandle.GetCpuHandle(); }
		Ideal::D3D12DescriptorHandle GetDescriptorHandle() { return m_srvHandle; }

	private:
		// 2024.04.21
		// Texture가 descriptor heap에 할당된 주소를 가지고 있는다.
		Ideal::D3D12DescriptorHandle m_srvHandle;
	};
}

