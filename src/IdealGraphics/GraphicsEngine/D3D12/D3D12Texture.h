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
			ComPtr<ID3D12Resource> Resource
		);

		void EmplaceSRV(Ideal::D3D12DescriptorHandle SRVHandle);

		// 2024.05.15 Texture가 SRV, RTV, DTV기능을 하기위해 다 집어넣겠다.
		// RTV 만들 때 필요한 것. Resource가 있어야 하고
		// D3D12_RENDER_TARGET_VIEW_DESC 이 있네?
		void EmplaceRTV(Ideal::D3D12DescriptorHandle RTVHandle);

		void EmplaceDSV(Ideal::D3D12DescriptorHandle DSVHandle);

		// 2024.04.21 : 디스크립터가 할당된 위치를 가져온다.
		//D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() { return m_srvHandle.GetCpuHandle(); }
		Ideal::D3D12DescriptorHandle GetSRV();
		Ideal::D3D12DescriptorHandle GetRTV();
		Ideal::D3D12DescriptorHandle GetDSV();
	private:
		// 2024.04.21
		// Texture가 descriptor heap에 할당된 주소를 가지고 있는다.
		Ideal::D3D12DescriptorHandle m_srvHandle;
		// 2024.05.14
		// RTV
		Ideal::D3D12DescriptorHandle m_rtvHandle;

		Ideal::D3D12DescriptorHandle m_dsvHandle;

	private:
		// 2024.05.15 Texture일 경우 필요한 여러가지 정보들
		//std::vector<std::shared_ptr<Ideal::
		//std::vector<std::shared_ptr<Ideal::D3D12View>> m_renderTargetViews;
	};
}

