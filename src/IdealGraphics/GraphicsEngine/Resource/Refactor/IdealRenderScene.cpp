#include "IdealRenderScene.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/Refactor/IdealSkinnedMeshObject.h"

#include "GraphicsEngine/D3D12/D3D12PipelineStateObject.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
Ideal::IdealRenderScene::IdealRenderScene()
{

}

Ideal::IdealRenderScene::~IdealRenderScene()
{

}

void Ideal::IdealRenderScene::Init(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	CreateStaticMeshPSO(Renderer);
	CreateSkinnedMeshPSO(Renderer);
}

void Ideal::IdealRenderScene::Draw(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	
	for (auto& m : m_staticMeshObjects)
	{
		// static mesh
		commandList->SetPipelineState(m_staticMeshPSO->GetPipelineState().Get());
		commandList->SetGraphicsRootSignature(m_staticMeshRootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m->Draw(d3d12Renderer);
	}
	for (auto& m : m_skinnedMeshObjects)
	{
		// skinned mesh
		commandList->SetPipelineState(m_skinnedMeshPSO->GetPipelineState().Get());
		commandList->SetGraphicsRootSignature(m_skinnedMeshRootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		m->Draw(Renderer);
	}
}

void Ideal::IdealRenderScene::AddObject(std::shared_ptr<Ideal::IMeshObject> MeshObject)
{
	if (std::dynamic_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject) != nullptr)
	{
		m_staticMeshObjects.push_back(std::static_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject));
	}
	else if (std::dynamic_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject) != nullptr)
	{
		m_skinnedMeshObjects.push_back(std::static_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject));
	}
}

void Ideal::IdealRenderScene::CreateStaticMeshPSO(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	
	//-------------------Sampler--------------------//
	CD3DX12_STATIC_SAMPLER_DESC sampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);
	//-------------------Range--------------------//
	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);

	//-------------------Parameter--------------------//
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];
	rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

	//-------------------Signature--------------------//
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Check(d3d12Renderer->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_staticMeshRootSignature.GetAddressOf())
	));

	m_staticMeshPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	m_staticMeshPSO->SetInputLayout(BasicVertex::InputElements, BasicVertex::InputElementCount);

	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	vs->CompileFromFile(L"../Shaders/BoxUV.hlsl", nullptr, nullptr, "VS", "vs_5_0");
	m_staticMeshPSO->SetVertexShader(vs);

	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	ps->CompileFromFile(L"../Shaders/BoxUV.hlsl", nullptr, nullptr, "PS", "ps_5_0");
	m_staticMeshPSO->SetPixelShader(ps);

	m_staticMeshPSO->SetRootSignature(m_staticMeshRootSignature.Get());
	m_staticMeshPSO->SetRasterizerState();
	m_staticMeshPSO->SetBlendState();
	m_staticMeshPSO->Create(d3d12Renderer);
}

void Ideal::IdealRenderScene::CreateSkinnedMeshPSO(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);

	//-------------------Sampler--------------------//
	CD3DX12_STATIC_SAMPLER_DESC sampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);
	//-------------------Range--------------------//
	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//-------------------Parameter--------------------//
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[2].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

	//-------------------Signature--------------------//
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Check(d3d12Renderer->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_skinnedMeshRootSignature.GetAddressOf())
	));

	m_skinnedMeshPSO = std::make_shared<Ideal::D3D12PipelineStateObject>();
	m_skinnedMeshPSO->SetInputLayout(SkinnedVertex::InputElements, SkinnedVertex::InputElementCount);

	std::shared_ptr<Ideal::D3D12Shader> vs = std::make_shared<Ideal::D3D12Shader>();
	vs->CompileFromFile(L"../Shaders/AnimationDefaultVS.hlsl", nullptr, nullptr, "VS", "vs_5_0");
	m_skinnedMeshPSO->SetVertexShader(vs);

	std::shared_ptr<Ideal::D3D12Shader> ps = std::make_shared<Ideal::D3D12Shader>();
	ps->CompileFromFile(L"../Shaders/AnimationDefaultVS.hlsl", nullptr, nullptr, "PS", "ps_5_0");
	m_skinnedMeshPSO->SetPixelShader(ps);

	m_skinnedMeshPSO->SetRootSignature(m_skinnedMeshRootSignature.Get());
	m_skinnedMeshPSO->SetRasterizerState();
	m_skinnedMeshPSO->SetBlendState();
	m_skinnedMeshPSO->Create(d3d12Renderer);
}
