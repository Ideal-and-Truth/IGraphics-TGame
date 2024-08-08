#include "GraphicsEngine/Resource/UI/IdealCanvas.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/Resource/IdealSprite.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/Resource/IdealSprite.h"

Ideal::IdealCanvas::IdealCanvas()
{

}

Ideal::IdealCanvas::~IdealCanvas()
{

}

void Ideal::IdealCanvas::Init(ComPtr<ID3D12Device> Device)
{
	CreateRootSignature(Device);
	CreatePSO(Device);
}

void Ideal::IdealCanvas::DrawCanvas(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> UIDescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool)
{
	// TODO Set
	CommandList->SetPipelineState(m_rectPSO.Get());
	CommandList->SetGraphicsRootSignature(m_rectRootSignature.Get());
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (auto& sprite : m_sprites)
	{
		sprite.lock()->DrawSprite(Device, CommandList, UIDescriptorHeap, CBPool);
	}
}

void Ideal::IdealCanvas::AddSprite(std::weak_ptr<Ideal::IdealSprite> Sprite)
{
	m_sprites.push_back(Sprite);
}

void Ideal::IdealCanvas::DeleteSprite(std::weak_ptr<Ideal::IdealSprite> Sprite)
{
	// TODO
}

void Ideal::IdealCanvas::CreateRootSignature(ComPtr<ID3D12Device> Device)
{
	CD3DX12_DESCRIPTOR_RANGE1 ranges[Ideal::RectRootSignature::Slot::Count];
	ranges[Ideal::RectRootSignature::Slot::SRV_Sprite].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		// t1 : Sprite
	ranges[Ideal::RectRootSignature::Slot::CBV_RectInfo].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : RectInfo

	CD3DX12_ROOT_PARAMETER1 rootParameters[Ideal::RectRootSignature::Slot::Count];
	rootParameters[Ideal::RectRootSignature::Slot::SRV_Sprite].InitAsDescriptorTable(1, &ranges[Ideal::RectRootSignature::Slot::SRV_Sprite]);
	rootParameters[Ideal::RectRootSignature::Slot::CBV_RectInfo].InitAsDescriptorTable(1, &ranges[Ideal::RectRootSignature::Slot::CBV_RectInfo]);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
	{
		// LinearWrapSampler
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC),	// s0
	};

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, staticSamplers, rootSignatureFlags); // (ARRAYSIZE(rootParameters), rootParameters, ARRAYSIZE(staticSamplers), staticSamplers);
	HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	Check(hr, L"Failed to serialize root signature in ui");
	//HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	//if (error)
	//{
	//	const wchar_t* msg = static_cast<wchar_t*>(error->GetBufferPointer());
	//	Check(hr, msg);
	//}

	hr = (Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_rectRootSignature)));
	Check(hr, L"Failed to create RootSignature");

	m_rectRootSignature->SetName(L"RectRootSignature");
}

void Ideal::IdealCanvas::CreatePSO(ComPtr<ID3D12Device> Device)
{
	// TODO : shader 
	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	vs->CompileFromFile(L"../Shaders/UI.hlsl", nullptr, nullptr, "VS", "vs_5_0");
	// TODO : shader
	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	ps->CompileFromFile(L"../Shaders/UI.hlsl", nullptr, nullptr, "PS", "ps_5_0");

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { SimpleVertex::InputElements, SimpleVertex::InputElementCount };
	psoDesc.pRootSignature = m_rectRootSignature.Get();
	psoDesc.VS = vs->GetShaderByteCode();
	psoDesc.PS = ps->GetShaderByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;

	Check(Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_rectPSO.GetAddressOf())));
	return;



	//m_rectPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	//m_rectPSO->SetInputLayout(SimpleVertex::InputElements, SimpleVertex::InputElementCount);
	//
	//// TODO : shader 
	//std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	//vs->CompileFromFile(L"../Shaders/UI.hlsl", nullptr, nullptr, "VS", "vs_5_0");
	//m_rectPSO->SetVertexShader(vs);
	//// TODO : shader
	//std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	//ps->CompileFromFile(L"../Shaders/UI.hlsl", nullptr, nullptr, "PS", "ps_5_0");
	//m_rectPSO->SetPixelShader(ps);
	//m_rectPSO->SetRootSignature(m_rectRootSignature.Get());
	//m_rectPSO->SetRasterizerState();
	//m_rectPSO->SetBlendState();
	//
	//DXGI_FORMAT rtvFormat[1] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	//DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
	//m_rectPSO->SetTargetFormat(1, rtvFormat, dsvFormat);
	//m_rectPSO->Create(Device.Get());
}
