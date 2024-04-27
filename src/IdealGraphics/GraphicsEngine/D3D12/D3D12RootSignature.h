#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"

namespace Ideal
{
	class D3D12RootSignature
	{
	public:
		D3D12RootSignature();
		virtual ~D3D12RootSignature();

	public:
		ComPtr<ID3D12RootSignature> GetRootSignature() { return m_rootSignature; }

	public:
		std::vector<CD3DX12_ROOT_PARAMETER1> m_rootParameters;
		ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}