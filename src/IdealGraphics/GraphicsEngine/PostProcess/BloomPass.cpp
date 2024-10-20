#include "GraphicsEngine/PostProcess/BloomPass.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12Viewport.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "GraphicsEngine/D3D12/DeferredDeleteManager.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include <d3d12.h>
#include <d3dx12.h>

void Ideal::BloomPass::InitBloomPass(ComPtr<ID3D12Device> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12Shader> DownSampleShader, std::shared_ptr<Ideal::D3D12Shader> BlurShader, std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> QuadMesh, uint32 Width, uint32 Height)
{
	m_width = Width;
	m_height = Height;
	// Texture 만들기 -> 다운샘플링
	Ideal::IdealTextureTypeFlag textureFlag = IDEAL_TEXTURE_SRV | IDEAL_TEXTURE_UAV;
	ResourceManager->CreateEmptyTexture2D(m_downSampleTexture0, Width * m_threshold, Height * m_threshold, DXGI_FORMAT_R16G16B16A16_FLOAT, textureFlag, L"PostProcess_BlurTexture0");
	// Texture 만들기 -> 블러처리
	ResourceManager->CreateEmptyTexture2D(m_blurTexture[0], Width * m_threshold, Height * m_threshold, DXGI_FORMAT_R16G16B16A16_FLOAT, textureFlag, L"PostProcess_BlurTexture0");
	ResourceManager->CreateEmptyTexture2D(m_blurTexture[1], Width * m_threshold, Height * m_threshold, DXGI_FORMAT_R16G16B16A16_FLOAT, textureFlag, L"PostProcess_BlurTexture0");

	m_quadMesh = QuadMesh;

	m_downSampleCS = DownSampleShader;
	m_blurCS = BlurShader;

	CreateDownSampleRootSignature(Device);
	CreateDownSamplePipelineState(Device);

	CreateBlurRootSignature(Device);
	CreateBlurPipelineState(Device);

	InitBlurCB();
}

void Ideal::BloomPass::PostProcess(std::shared_ptr<Ideal::D3D12Texture> BeforeTexture, std::shared_ptr<Ideal::D3D12Texture> BloomTexture, std::shared_ptr<Ideal::D3D12Texture> RenderTarget, std::shared_ptr<Ideal::D3D12Viewport> Viewport, ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool)
{
	{
		PostProcess_DownSample(BloomTexture, m_downSampleTexture0, m_threshold, Device, CommandList, DescriptorHeap, CBPool, m_width, m_height);
	}

	{
		uint32 direction = 0;
		PostProcess_Blur(m_downSampleTexture0, m_blurTexture[0], direction, Device, CommandList, DescriptorHeap, CBPool, m_width, m_height);
		direction = 1;
		PostProcess_Blur(m_blurTexture[0], m_blurTexture[1], direction, Device, CommandList, DescriptorHeap, CBPool, m_width, m_height);
	}
}

void Ideal::BloomPass::Resize(uint32 Width, uint32 Height, std::shared_ptr<DeferredDeleteManager> DeferredDeleteManager, std::shared_ptr<Ideal::ResourceManager> ResourceManager)
{
	m_width = Width; 
	m_height = Height;


	Ideal::IdealTextureTypeFlag textureFlag = IDEAL_TEXTURE_SRV | IDEAL_TEXTURE_UAV;
	if (m_downSampleTexture0)
	{
		DeferredDeleteManager->AddTextureToDeferredDelete(m_downSampleTexture0);
		m_downSampleTexture0 = std::make_shared<Ideal::D3D12Texture>();
		ResourceManager->CreateEmptyTexture2D(m_downSampleTexture0, Width * m_threshold, Height * m_threshold, DXGI_FORMAT_R16G16B16A16_FLOAT, textureFlag, L"PostProcess_BlurTexture0");
	}
	for (uint32 i = 0; i < 2; ++i)
	{
		DeferredDeleteManager->AddTextureToDeferredDelete(m_blurTexture[i]);
		m_blurTexture[i] = std::make_shared<Ideal::D3D12Texture>();
		ResourceManager->CreateEmptyTexture2D(m_blurTexture[i], Width * m_threshold, Height * m_threshold, DXGI_FORMAT_R16G16B16A16_FLOAT, textureFlag, L"PostProcess_BlurTexture0");
	}
}

void Ideal::BloomPass::Free()
{
	m_downSampleTexture0->Free();
	m_blurTexture[0]->Free();
	m_blurTexture[1]->Free();
}

std::shared_ptr<Ideal::D3D12Texture> Ideal::BloomPass::GetBlurTexture()
{
	return m_blurTexture[1];
}


void Ideal::BloomPass::CreateDownSampleRootSignature(ComPtr<ID3D12Device> Device)
{
	CD3DX12_DESCRIPTOR_RANGE ranges[Ideal::PostProcessDownSampleRootSignature::Slot::Count];
	ranges[Ideal::PostProcessDownSampleRootSignature::Slot::SRV_Input].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		// t0 : srv input
	ranges[Ideal::PostProcessDownSampleRootSignature::Slot::UAV_Output].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);	// u0 : uav output
	ranges[Ideal::PostProcessDownSampleRootSignature::Slot::CBV_Treshold].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : cbv threshold

	CD3DX12_ROOT_PARAMETER rootParameter[Ideal::PostProcessDownSampleRootSignature::Slot::Count];
	rootParameter[Ideal::PostProcessDownSampleRootSignature::Slot::SRV_Input].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessDownSampleRootSignature::Slot::SRV_Input]);
	rootParameter[Ideal::PostProcessDownSampleRootSignature::Slot::UAV_Output].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessDownSampleRootSignature::Slot::UAV_Output]);
	rootParameter[Ideal::PostProcessDownSampleRootSignature::Slot::CBV_Treshold].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessDownSampleRootSignature::Slot::CBV_Treshold]);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		ARRAYSIZE(rootParameter), rootParameter, 0, nullptr
	);

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	Check(hr, L"Failed to serialize DownSampleRootSignature");
	hr = Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(m_downSampleRootSignature.GetAddressOf()));
	Check(hr, L"Faield to create DownSampleRootSignature");

	m_downSampleRootSignature->SetName(L"DownSampleRootSignature");
}

void Ideal::BloomPass::CreateDownSamplePipelineState(ComPtr<ID3D12Device> Device)
{

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
	computePipelineStateDesc.pRootSignature = m_downSampleRootSignature.Get();
	CD3DX12_SHADER_BYTECODE shader(m_downSampleCS->GetBufferPointer(), m_downSampleCS->GetSize());
	computePipelineStateDesc.CS = shader;
	Check(
		Device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_downSamplePipelineState)),
		L"Failed to create DownSmaple Compute Pipeline State"
	);
}

void Ideal::BloomPass::CreateBlurRootSignature(ComPtr<ID3D12Device> Device)
{
	CD3DX12_DESCRIPTOR_RANGE ranges[Ideal::PostProcessBlurRootSignature::Slot::Count];
	ranges[Ideal::PostProcessBlurRootSignature::Slot::SRV_Input].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		// t0 : srv input
	ranges[Ideal::PostProcessBlurRootSignature::Slot::UAV_Output].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);	// u0 : uav output
	ranges[Ideal::PostProcessBlurRootSignature::Slot::CBV_BlurParameter].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : cbv blur

	CD3DX12_ROOT_PARAMETER rootParameter[Ideal::PostProcessBlurRootSignature::Slot::Count];
	rootParameter[Ideal::PostProcessBlurRootSignature::Slot::SRV_Input].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessBlurRootSignature::Slot::SRV_Input]);
	rootParameter[Ideal::PostProcessBlurRootSignature::Slot::UAV_Output].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessBlurRootSignature::Slot::UAV_Output]);
	rootParameter[Ideal::PostProcessBlurRootSignature::Slot::CBV_BlurParameter].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessBlurRootSignature::Slot::CBV_BlurParameter]);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		ARRAYSIZE(rootParameter), rootParameter, 0, nullptr
	);

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	Check(hr, L"Failed to serialize BlurRootSignature");
	hr = Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(m_blurRootSignature.GetAddressOf()));
	Check(hr, L"Faield to create BlurRootSignature");

	m_blurRootSignature->SetName(L"BlurRootSignature");
}

void Ideal::BloomPass::CreateBlurPipelineState(ComPtr<ID3D12Device> Device)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
	computePipelineStateDesc.pRootSignature = m_blurRootSignature.Get();
	CD3DX12_SHADER_BYTECODE shader(m_blurCS->GetBufferPointer(), m_blurCS->GetSize());
	computePipelineStateDesc.CS = shader;
	Check(
		Device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_blurPipelineState)),
		L"Failed to create Blur Compute Pipeline State"
	);
}

void Ideal::BloomPass::PostProcess_DownSample(std::shared_ptr<Ideal::D3D12Texture> BloomTexture, std::shared_ptr<Ideal::D3D12Texture> OutTexture, float Threshold, ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, uint32 Width, uint32 Height)
{
	CommandList->SetComputeRootSignature(m_downSampleRootSignature.Get());
	CommandList->SetPipelineState(m_downSamplePipelineState.Get());

	// Bind to Shader
	{
		// Parameter : SRV_INPUT
		auto handle = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), BloomTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetComputeRootDescriptorTable(Ideal::PostProcessDownSampleRootSignature::Slot::SRV_Input, handle.GetGpuHandle());
	}
	{
		// Parameter : UAV_OUTPUT
		auto handle = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), OutTexture->GetUAV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetComputeRootDescriptorTable(Ideal::PostProcessDownSampleRootSignature::Slot::UAV_Output, handle.GetGpuHandle());
	}
	{
		// Parameter : CBV_Threshold
		auto handle = DescriptorHeap->Allocate();
		auto cb = CBPool->Allocate(Device.Get(), sizeof(float));
		memcpy(cb->SystemMemAddr, &Threshold, sizeof(float));
		Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetComputeRootDescriptorTable(Ideal::PostProcessDownSampleRootSignature::Slot::CBV_Treshold, handle.GetGpuHandle());
	}

	// Barrier
	CD3DX12_RESOURCE_BARRIER barrierBloomTextureBarrier0 = CD3DX12_RESOURCE_BARRIER::Transition(
		BloomTexture->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	CommandList->ResourceBarrier(1, &barrierBloomTextureBarrier0);

	CD3DX12_RESOURCE_BARRIER outputBarrier0 = CD3DX12_RESOURCE_BARRIER::Transition(
		OutTexture->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	CommandList->ResourceBarrier(1, &outputBarrier0);
	CommandList->Dispatch((float)Width / 16, (float)Height / 16, 1);

	//Barrier 
	CD3DX12_RESOURCE_BARRIER barrierBloomTextureBarrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
		BloomTexture->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	CommandList->ResourceBarrier(1, &barrierBloomTextureBarrier1);

	CD3DX12_RESOURCE_BARRIER outputBarrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
		OutTexture->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	CommandList->ResourceBarrier(1, &outputBarrier1);
}

void Ideal::BloomPass::PostProcess_Blur(std::shared_ptr<Ideal::D3D12Texture> DownSampleTexture, std::shared_ptr<Ideal::D3D12Texture> OutBlurTexture, uint32 direction, ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, uint32 Width, uint32 Height)
{
	CommandList->SetComputeRootSignature(m_blurRootSignature.Get());
	CommandList->SetPipelineState(m_blurPipelineState.Get());

	{
		// Bind to Shader
		{
			// Parameter : SRV_INPUT
			auto handle = DescriptorHeap->Allocate();
			Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), DownSampleTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CommandList->SetComputeRootDescriptorTable(Ideal::PostProcessBlurRootSignature::Slot::SRV_Input, handle.GetGpuHandle());
		}
		{
			// Parameter : UAV_OUTPUT
			auto handle = DescriptorHeap->Allocate();
			Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), OutBlurTexture->GetUAV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CommandList->SetComputeRootDescriptorTable(Ideal::PostProcessBlurRootSignature::Slot::UAV_Output, handle.GetGpuHandle());
		}
		{
			m_cbBlur.direction = direction;

			// Parameter : CBV_BlurParam
			auto handle = DescriptorHeap->Allocate();
			auto cb = CBPool->Allocate(Device.Get(), sizeof(CB_Blur));
			memcpy(cb->SystemMemAddr, &m_cbBlur, sizeof(CB_Blur));
			Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CommandList->SetComputeRootDescriptorTable(Ideal::PostProcessBlurRootSignature::Slot::CBV_BlurParameter, handle.GetGpuHandle());
		}

		CommandList->Dispatch((float)Width / 16, (float)Height / 16, 1);

		// Barrier
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			OutBlurTexture->GetResource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		CommandList->ResourceBarrier(1, &barrier);
	}
}

void Ideal::BloomPass::InitBlurCB()
{
	// https://github.com/aaronscherzinger/directx11_bloom/blob/master/src/main.cpp

	m_cbBlur.radius = GAUSSIAN_RADIUS;
	m_cbBlur.direction = 0;
	
	// compute Gaussian kernel
	float sigma = 10.f;
	float sigmaRcp = 1.f / sigma;
	float twoSigmaSq = 2 * sigma * sigma;

	float sum = 0.f;
	for (uint32 i = 0; i <= GAUSSIAN_RADIUS; ++i)
	{
		// we omit the normalization factor here for the discrete version and normalize using the sum afterwards
		m_cbBlur.coefficients[i] = (1.f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
		sum += 2 * m_cbBlur.coefficients[i];
	}

	// the center (index 0) has been counted twice, so we subtract it once
	sum -= m_cbBlur.coefficients[0];

	// we normalize all entries using the sum so that the entire kernel gives us a sum of coefficients = 0
	float normalizationFactor = 1.f / sum;
	for (size_t i = 0; i <= GAUSSIAN_RADIUS; ++i)
	{
		m_cbBlur.coefficients[i] *= normalizationFactor;
	}
}
