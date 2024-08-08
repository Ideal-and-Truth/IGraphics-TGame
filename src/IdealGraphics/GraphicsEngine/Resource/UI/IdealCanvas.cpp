#include "GraphicsEngine/Resource/UI/IdealCanvas.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/VertexInfo.h"
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

void Ideal::IdealCanvas::DrawCanvas()
{
	for (auto& sprite : m_sprites)
	{

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
	CD3DX12_DESCRIPTOR_RANGE ranges[Ideal::RectRootSignature::Slot::Count];
	ranges[Ideal::RectRootSignature::Slot::SRV_Sprite].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		// t1 : Sprite
	ranges[Ideal::RectRootSignature::Slot::CBV_RectInfo].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : RectInfo

	CD3DX12_ROOT_PARAMETER rootParameters[Ideal::RectRootSignature::Slot::Count];
	rootParameters[Ideal::RectRootSignature::Slot::SRV_Sprite].InitAsDescriptorTable(1, &ranges[Ideal::RectRootSignature::Slot::SRV_Sprite]);
	rootParameters[Ideal::RectRootSignature::Slot::CBV_RectInfo].InitAsDescriptorTable(1, &ranges[Ideal::RectRootSignature::Slot::CBV_RectInfo]);

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
	{
		// LinearWrapSampler
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC),	// s0
	};

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters, ARRAYSIZE(staticSamplers), staticSamplers);

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	if (error)
	{
		const wchar_t* msg = static_cast<wchar_t*>(error->GetBufferPointer());
		Check(hr, msg);
	}

	hr = (Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_rectRootSignature)));
	Check(hr, L"Failed to create RootSignature");

	m_rectRootSignature->SetName(L"Rect");
}

void Ideal::IdealCanvas::CreatePSO(ComPtr<ID3D12Device> Device)
{
	m_rectPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	m_rectPSO->SetInputLayout(QuadVertex::InputElements, QuadVertex::InputElementCount);

	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	// TODO : shader 
	m_rectPSO->SetVertexShader(vs);
	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	// TODO : shader
	m_rectPSO->SetPixelShader(ps);
	m_rectPSO->SetRootSignature(m_rectRootSignature.Get());
	m_rectPSO->SetRasterizerState();
	m_rectPSO->SetBlendState();

	DXGI_FORMAT rtvFormat[1] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_rectPSO->SetTargetFormat(1, rtvFormat, dsvFormat);
	m_rectPSO->Create(Device.Get());
}
