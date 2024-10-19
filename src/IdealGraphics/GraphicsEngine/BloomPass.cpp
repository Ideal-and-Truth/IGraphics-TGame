#include "GraphicsEngine/BloomPass.h"
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

void Ideal::BloomPass::InitBloomPass(ComPtr<ID3D12Device> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12Shader> DownSampleShader, std::shared_ptr<Ideal::D3D12Shader> BlurShaderPS, std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> QuadMesh, uint32 Width, uint32 Height)
{
	m_width = Width;
	m_height = Height;
	// Texture 만들기 -> 다운샘플링
	Ideal::IdealTextureTypeFlag textureFlag = IDEAL_TEXTURE_SRV | IDEAL_TEXTURE_UAV;
	ResourceManager->CreateEmptyTexture2D(m_blurTexture0, Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT, textureFlag, L"BlurTexture0");

	m_quadMesh = QuadMesh;

	m_downSampleCS = DownSampleShader;
	m_blurShaderPS = BlurShaderPS;

	CreateDownSampleRootSignature(Device);
	CreateDownSamplePipelineState(Device);
}

void Ideal::BloomPass::PostProcess(std::shared_ptr<Ideal::D3D12Texture> BeforeTexture, std::shared_ptr<Ideal::D3D12Texture> BloomTexture, std::shared_ptr<Ideal::D3D12Texture> RenderTarget, std::shared_ptr<Ideal::D3D12Viewport> Viewport, ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool)
{
	{
		// half 
		float threshold = 0.5f;
		PostProcess_DownSample(BloomTexture, m_blurTexture0, threshold, Device, CommandList, DescriptorHeap, CBPool, m_width, m_height);
	}
}

void Ideal::BloomPass::Resize(uint32 Width, uint32 Height, std::shared_ptr<DeferredDeleteManager> DeferredDeleteManager, std::shared_ptr<Ideal::ResourceManager> ResourceManager)
{
	m_width = Width; 
	m_height = Height;

	if (m_blurTexture0)
	{
		//m_viewportBlur0->ReSize(Width / 4.f, Height / 4.f);
		DeferredDeleteManager->AddTextureToDeferredDelete(m_blurTexture0);
		Ideal::IdealTextureTypeFlag textureFlag = IDEAL_TEXTURE_SRV | IDEAL_TEXTURE_UAV;
		m_blurTexture0 = std::make_shared<Ideal::D3D12Texture>();
		ResourceManager->CreateEmptyTexture2D(m_blurTexture0, Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT, textureFlag, L"BlurTexture0");
	}
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

	CD3DX12_RESOURCE_BARRIER barrier0 = CD3DX12_RESOURCE_BARRIER::Transition(
		OutTexture->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	CommandList->ResourceBarrier(1, &barrier0);
	CommandList->Dispatch((float)Width / 16, (float)Height / 16, 1);

	//Barrier 
	CD3DX12_RESOURCE_BARRIER barrierBloomTextureBarrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
		BloomTexture->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	CommandList->ResourceBarrier(1, &barrierBloomTextureBarrier1);

	CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
		OutTexture->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	CommandList->ResourceBarrier(1, &barrier1);
}
