#include "GraphicsEngine/PostProcess/CompositePass.h"
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


void Ideal::CompositePass::InitCompositePass(std::shared_ptr<Ideal::D3D12Shader> CompositeVS, std::shared_ptr<Ideal::D3D12Shader> CompositePS, ComPtr<ID3D12Device> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> QuadMesh, uint32 Width, uint32 Height)
{
	m_compositeVS = CompositeVS;
	m_compositePS = CompositePS;

	m_quadMesh = QuadMesh;

	CreateCompositeRootSignature(Device);
	CreateCompositePipelineState(Device);
}

void Ideal::CompositePass::PostProcess(std::shared_ptr<Ideal::D3D12Texture> OriginTexture, std::shared_ptr<Ideal::D3D12Texture> BlurTexture, std::shared_ptr<Ideal::D3D12Texture> DepthBuffer, std::shared_ptr<Ideal::D3D12Texture> RenderTarget, std::shared_ptr<Ideal::D3D12Texture> NewDepth, std::shared_ptr<Ideal::D3D12Viewport> Viewport, ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, uint32 CurrentContextIndex)
{
	CommandList->RSSetViewports(1, &Viewport->GetViewport());
	CommandList->RSSetScissorRects(1, &Viewport->GetScissorRect());
	
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2] = { RenderTarget->GetRTV().GetCpuHandle(), NewDepth->GetRTV().GetCpuHandle() };
	CommandList->OMSetRenderTargets(2, rtvHandle, FALSE, nullptr);
	//CommandList->OMSetRenderTargets(1, &RenderTarget->GetRTV().GetCpuHandle(), FALSE, nullptr);


	CommandList->SetGraphicsRootSignature(m_compositeRootSignature.Get());
	CommandList->SetPipelineState(m_compositePipelineState.Get());

	const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = m_quadMesh->GetVertexBufferView();
	const D3D12_INDEX_BUFFER_VIEW& indexBufferView = m_quadMesh->GetIndexBufferView();

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	CommandList->IASetIndexBuffer(&indexBufferView);

	// Bind Shader
	{
		// Parameter : SRV_Origin
		auto handle = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), OriginTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::PostProcessCompositeRootSignature::Slot::SRV_Origin, handle.GetGpuHandle());
	}
	{
		// Parameter : SRV_Blur
		auto handle = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), BlurTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::PostProcessCompositeRootSignature::Slot::SRV_Blur, handle.GetGpuHandle());
	}
	{
		// Parameter : SRV_Depth
		auto handle = DescriptorHeap->Allocate();
		Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), DepthBuffer->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::PostProcessCompositeRootSignature::Slot::SRV_Depth, handle.GetGpuHandle());
	}
	{
		//https://github.com/aaronscherzinger/directx11_bloom/blob/master/src/main.cpp
		// 336줄
		// Parameter : CBV_Bloom
		auto handle = DescriptorHeap->Allocate();
		float compParams = 0.75f;	// Temp
		auto cb = CBPool->Allocate(Device.Get(), sizeof(float));
		memcpy(cb->SystemMemAddr, &compParams, sizeof(float));
		Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetGraphicsRootDescriptorTable(Ideal::PostProcessCompositeRootSignature::Slot::CBV_Bloom, handle.GetGpuHandle());
	}

	// Draw
	CommandList->DrawIndexedInstanced(m_quadMesh->GetElementCount(), 1, 0, 0, 0);
}

void Ideal::CompositePass::CreateCompositeRootSignature(ComPtr<ID3D12Device> Device)
{
	CD3DX12_DESCRIPTOR_RANGE1 ranges[Ideal::PostProcessCompositeRootSignature::Slot::Count];
	ranges[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Origin].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 : Origin
	ranges[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Blur].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 : blur
	ranges[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Depth].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // t2 : depth
	ranges[Ideal::PostProcessCompositeRootSignature::Slot::CBV_Bloom].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0

	CD3DX12_ROOT_PARAMETER1 rootParameters[Ideal::PostProcessCompositeRootSignature::Slot::Count];
	rootParameters[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Origin].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Origin]);
	rootParameters[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Blur].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Blur]);
	rootParameters[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Depth].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessCompositeRootSignature::Slot::SRV_Depth]);
	rootParameters[Ideal::PostProcessCompositeRootSignature::Slot::CBV_Bloom].InitAsDescriptorTable(1, &ranges[Ideal::PostProcessCompositeRootSignature::Slot::CBV_Bloom]);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
	{
		// LinearWrapSampler
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC),	// s0
	};

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(staticSamplers), staticSamplers, rootSignatureFlags);
	HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	Check(hr, L"Failed to serialize Composite RootSignature");
	hr = (Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_compositeRootSignature)));
	Check(hr, L"Failed to create Composite RootSignature");

	m_compositeRootSignature->SetName(L"CompositeRootSignature");
}

void Ideal::CompositePass::CreateCompositePipelineState(ComPtr<ID3D12Device> Device)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { SimpleVertex::InputElements, SimpleVertex::InputElementCount };
	psoDesc.pRootSignature = m_compositeRootSignature.Get();
	psoDesc.VS = m_compositeVS->GetShaderByteCode();
	psoDesc.PS = m_compositePS->GetShaderByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	{
		// https://codingfarm.tistory.com/609
		// 설정된 블렌드 상태
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		
		psoDesc.BlendState = blendDesc;
	}
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	{
		psoDesc.NumRenderTargets = 2;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// Composite
		psoDesc.RTVFormats[1] = DXGI_FORMAT_R32_FLOAT;	// Depth
	}
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;

	Check(
		Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_compositePipelineState.GetAddressOf())),
		L"Failed to Create Composite Pipeline State"
	);
}
