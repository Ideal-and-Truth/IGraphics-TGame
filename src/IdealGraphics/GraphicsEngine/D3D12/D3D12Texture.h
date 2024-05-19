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
		// ResourceManager���� ȣ��ȴ�.
		void Create(
			ComPtr<ID3D12Resource> Resource
		);

		void EmplaceSRV(Ideal::D3D12DescriptorHandle SRVHandle);

		// 2024.05.15 Texture�� SRV, RTV, DTV����� �ϱ����� �� ����ְڴ�.
		// RTV ���� �� �ʿ��� ��. Resource�� �־�� �ϰ�
		// D3D12_RENDER_TARGET_VIEW_DESC �� �ֳ�?
		void EmplaceRTV(Ideal::D3D12DescriptorHandle RTVHandle);

		void EmplaceDSV(Ideal::D3D12DescriptorHandle DSVHandle);

		// 2024.04.21 : ��ũ���Ͱ� �Ҵ�� ��ġ�� �����´�.
		//D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() { return m_srvHandle.GetCpuHandle(); }
		Ideal::D3D12DescriptorHandle GetSRV();
		Ideal::D3D12DescriptorHandle GetRTV();
		Ideal::D3D12DescriptorHandle GetDSV();
	private:
		// 2024.04.21
		// Texture�� descriptor heap�� �Ҵ�� �ּҸ� ������ �ִ´�.
		Ideal::D3D12DescriptorHandle m_srvHandle;
		// 2024.05.14
		// RTV
		Ideal::D3D12DescriptorHandle m_rtvHandle;

		Ideal::D3D12DescriptorHandle m_dsvHandle;

	private:
		// 2024.05.15 Texture�� ��� �ʿ��� �������� ������
		//std::vector<std::shared_ptr<Ideal::
		//std::vector<std::shared_ptr<Ideal::D3D12View>> m_renderTargetViews;
	};
}

