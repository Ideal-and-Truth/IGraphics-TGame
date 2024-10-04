#include "GenerateMips.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"


void Ideal::GenerateMips::Init(ComPtr<ID3D12Device> Device, std::shared_ptr<Ideal::D3D12Shader> Shader)
{
	// Set Shader
	m_GenerateMipsCS = Shader;

	CreateRootSignature(Device);
	CreatePipelineState(Device);
}

void Ideal::GenerateMips::Generate(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, std::shared_ptr<Ideal::D3D12Texture> Texture, CB_GenerateMipsInfo* generateMipsCB)
{
	// ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList,std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool,

	// TODO : Dispatch
	// NEED : Device, CommandList, CBPool

	CommandList->SetComputeRootSignature(m_rootSignature.Get());
	CommandList->SetPipelineState(m_pipelineState.Get());
	CommandList->SetDescriptorHeaps(1, DescriptorHeap->GetDescriptorHeap().GetAddressOf());

	auto resourceDesc = Texture->GetResource()->GetDesc();

	for (uint32 srcMip = 0; srcMip < resourceDesc.MipLevels - 1;)
	{
		uint64 srcWidth = resourceDesc.Width >> srcMip;
		uint64 srcHeight = resourceDesc.Height >> srcMip;
		uint64 dstWidth = static_cast<uint32>(srcWidth >> 1);
		uint64 dstHeight = srcHeight >> 1;
		// 0b00(0): Both width and height are even.
		// 0b01(1): Width is odd, height is even.
		// 0b10(2): Width is even, height is odd.
		// 0b11(3): Both width and height are odd.
		generateMipsCB->SrcDimension = (srcHeight & 1) << 1 | (srcWidth & 1);
		DWORD mipCount;
		_BitScanForward(&mipCount, (dstWidth == 1 ? dstHeight : dstWidth) |
			(dstHeight == 1 ? dstWidth : dstHeight));

		mipCount = (srcMip + mipCount) >= resourceDesc.MipLevels ? resourceDesc.MipLevels - srcMip - 1 : mipCount;

		dstWidth = std::max<DWORD>(1, dstWidth);
		dstHeight = std::max<DWORD>(1, dstHeight);
		
		generateMipsCB->SrcMipLevel = srcMip;
		generateMipsCB->NumMipLevels = mipCount;
		generateMipsCB->TexelSize.x = 1.0f / (float)dstWidth;
		generateMipsCB->TexelSize.y = 1.0f / (float)dstHeight;

		auto handle0 = DescriptorHeap->Allocate();
		auto cb0 = CBPool->Allocate(Device.Get(), sizeof(CB_GenerateMipsInfo));
		memcpy(cb0->SystemMemAddr, &generateMipsCB, sizeof(CB_GenerateMipsInfo));
		Device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), cb0->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetComputeRootDescriptorTable(Ideal::GenerateMipsCSRootSignature::Slot::CBV_GenerateMipsInfo, handle0.GetGpuHandle());

		// Parameter1 : SRV_SourceMip
		auto handle1 = DescriptorHeap->Allocate();
		auto& sourceTextureSRV = Texture->GetSRV();
		Device->CopyDescriptorsSimple(1, handle1.GetCpuHandle(), sourceTextureSRV.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetComputeRootDescriptorTable(Ideal::GenerateMipsCSRootSignature::Slot::SRV_SourceMip, handle1.GetGpuHandle());

		for (uint32 mip = 0; mip < mipCount; ++mip)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = resourceDesc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = srcMip + mip + 1;

			auto handle2to5 = DescriptorHeap->Allocate();
			auto& outMipUAV = Texture->GetUAV(mip);
			Device->CopyDescriptorsSimple(1, handle2to5.GetCpuHandle(), outMipUAV.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CommandList->SetComputeRootDescriptorTable(Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip1 + mip, handle2to5.GetGpuHandle());
		}

		CommandList->Dispatch(dstWidth * 0.8f, dstHeight * 0.8f, 1);
		CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(Texture->GetResource());
		CommandList->ResourceBarrier(1, &uavBarrier);

		srcMip += mipCount;
	}
}

void Ideal::GenerateMips::CreateRootSignature(ComPtr<ID3D12Device> Device)
{
	CD3DX12_DESCRIPTOR_RANGE ranges[Ideal::GenerateMipsCSRootSignature::Slot::Count];
	ranges[Ideal::GenerateMipsCSRootSignature::Slot::CBV_GenerateMipsInfo].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0 : GenerateMip Info
	ranges[Ideal::GenerateMipsCSRootSignature::Slot::SRV_SourceMip].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 : SrcMip
	ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);	// u0 : OutMip1
	ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);	// u0 : OutMip2
	ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);	// u0 : OutMip3
	ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);	// u0 : OutMip4

	CD3DX12_ROOT_PARAMETER rootParameters[Ideal::GenerateMipsCSRootSignature::Slot::Count];
	rootParameters[Ideal::GenerateMipsCSRootSignature::Slot::CBV_GenerateMipsInfo].InitAsDescriptorTable(1, &ranges[Ideal::GenerateMipsCSRootSignature::Slot::CBV_GenerateMipsInfo]);
	rootParameters[Ideal::GenerateMipsCSRootSignature::Slot::SRV_SourceMip].InitAsDescriptorTable(1, &ranges[Ideal::GenerateMipsCSRootSignature::Slot::SRV_SourceMip]);
	rootParameters[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip1].InitAsDescriptorTable(1, &ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip1]);
	rootParameters[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip2].InitAsDescriptorTable(1, &ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip2]);
	rootParameters[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip3].InitAsDescriptorTable(1, &ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip3]);
	rootParameters[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip4].InitAsDescriptorTable(1, &ranges[Ideal::GenerateMipsCSRootSignature::Slot::UAV_OutMip4]);

	CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		ARRAYSIZE(rootParameters), rootParameters, 1, &linearClampSampler
	);

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	if (error)
	{
		const wchar_t* msg = static_cast<wchar_t*>(error->GetBufferPointer());
		Check(hr, msg);
	}
	Check(
		Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature))
		, L"Failed to Create GenerateMips RootSignature"
		);

	m_rootSignature->SetName(L"GenerateMipsRootSignature");
}

void Ideal::GenerateMips::CreatePipelineState(ComPtr<ID3D12Device> Device)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
	computePipelineStateDesc.pRootSignature = m_rootSignature.Get();
	computePipelineStateDesc.CS = m_GenerateMipsCS->GetShaderByteCode();

	Check(
		Device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_pipelineState)),
			L"Failed to Create GenerateMips PipelineState"
	);
}
