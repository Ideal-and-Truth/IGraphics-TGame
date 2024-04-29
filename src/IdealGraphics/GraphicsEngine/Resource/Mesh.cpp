#include "GraphicsEngine/Resource/Mesh.h"
#include "GraphicsEngine/Resource/Material.h"
#include "GraphicsEngine/Resource/Model.h"

#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"

Ideal::Mesh::Mesh()
{

}

Ideal::Mesh::~Mesh()
{

}

void Ideal::Mesh::Create(std::shared_ptr<D3D12Renderer> Renderer)
{
	std::shared_ptr<Ideal::ResourceManager> resourceManager = Renderer->GetResourceManager();
	
	//--------------Init---------------//
	//InitRootSignature(Renderer);
	//InitPipelineState(Renderer);
	InitRootSignature2(Renderer);
	InitPipelineState2(Renderer);

	//--------------VB---------------//
	// 2024.04.22 : Renderer�� ������ �ִ� ResourceManager�� �̿��Ͽ� ����� �Ͽ���.
	{
		m_vertexBuffer = std::make_shared<D3D12VertexBuffer>();
		resourceManager->CreateVertexBuffer<BasicVertex>(m_vertexBuffer, m_vertices);
	}

	//--------------IB---------------//
	// 2024.04.22 : Renderer�� ������ �ִ� ResourceManager�� �̿��Ͽ� ����� �Ͽ���.
	{
		m_indexBuffer = std::make_shared<D3D12IndexBuffer>();
		resourceManager->CreateIndexBuffer(m_indexBuffer, m_indices);
	}

	//--------------CB---------------//
	{
		const uint32 bufferSize = sizeof(CB_Transform);

		m_constantBuffer.Create(Renderer->GetDevice().Get(), bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
	}

	//-----------Clear Data-----------//

	m_vertices.clear();
	m_indices.clear();

	//--------------Material---------------//
	if (m_material)
	{
		// 2024.04.21 : �׳� Renderer�� �����ָ� Allocate�� ���� ���� Offset ��������� �ʾƵ� �˾Ƽ� �Ҵ��Ѵ�.
		m_material->Create(Renderer);
	}

	m_transform.World = Matrix::Identity;
}

void Ideal::Mesh::Tick(std::shared_ptr<D3D12Renderer> Renderer)
{
	/*static float rot = 0.f;
	rot += 0.2f;*/
	m_transform.WorldInvTranspose = m_transform.World.Invert().Transpose();
	m_transform.View = Renderer->GetView();// .Transpose();
	m_transform.Proj = Renderer->GetProj();// .Transpose();

	//m_transform.World = Matrix::CreateRotationY(DirectX::XMConvertToRadians(rot)) * Matrix::CreateTranslation(Vector3(0.f, 0.f, -800.f));
	//
	CB_Transform* t = (CB_Transform*)m_constantBuffer.GetMappedMemory(Renderer->GetFrameIndex());
	*t = m_transform;
}

void Ideal::Mesh::Render(std::shared_ptr<D3D12Renderer> Renderer)
{
	ID3D12GraphicsCommandList* CommandList = Renderer->GetCommandList().Get();

	CommandList->SetPipelineState(m_pipelineState.Get());
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = m_vertexBuffer->GetView();
	CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	const D3D12_INDEX_BUFFER_VIEW& indexBufferView = m_indexBuffer->GetView();
	CommandList->IASetIndexBuffer(&indexBufferView);

	//-------------------Root Signature--------------------//
	CommandList->SetGraphicsRootSignature(m_rootSignature.Get());
	//ID3D12DescriptorHeap* heaps[] = { Renderer->GetSRVDescriptorHeap().Get() };
	//CommandList->SetDescriptorHeaps(_countof(heaps), heaps);

	// 2024.04.20 �ι�° srv�� ����!!!!!!
	//CD3DX12_GPU_DESCRIPTOR_HANDLE handler = CD3DX12_GPU_DESCRIPTOR_HANDLE(Renderer->GetSRVDescriptorHeap().Get()->GetGPUDescriptorHandleForHeapStart());
	//handler.Offset(m_descriptorIncrementSize);
	//CommandList->SetGraphicsRootDescriptorTable(0, handler);
	
	// 2024.04.21 �ٽ� ���� Material�� ������ �ִ� Texture���� Descriptor Handle�� ����
	m_material->BindToShader(Renderer);

	//CommandList->SetGraphicsRootDescriptorTable(0, Renderer->GetSRVDescriptorHeap().Get()->GetGPUDescriptorHandleForHeapStart());

	uint32 currentIndex = Renderer->GetFrameIndex();
	CommandList->SetGraphicsRootConstantBufferView(STATIC_MESH_ROOT_CONSTANT_INDEX, m_constantBuffer.GetGPUVirtualAddress(currentIndex));
	CommandList->DrawIndexedInstanced(m_indexBuffer->GetElementCount(), 1, 0, 0, 0);
}

void Ideal::Mesh::SetTransformMatrix(const Matrix& transform)
{
	m_transform.World = transform;
}

void Ideal::Mesh::InitRootSignature(std::shared_ptr<D3D12Renderer> Renderer)
{
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];

	rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Check(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Check(Renderer->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.GetAddressOf())
	));
}

void Ideal::Mesh::InitPipelineState(std::shared_ptr<D3D12Renderer> Renderer)
{
	ComPtr<ID3DBlob> errorBlob;
	uint32 compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// Set VS
	ComPtr<ID3DBlob> vertexShader;
	Check(D3DCompileFromFile(
		L"Shaders/BoxVS.hlsl",
		nullptr,
		nullptr,
		"VS",
		"vs_5_0",
		compileFlags, 0, &vertexShader, nullptr));

	// Set PS
	ComPtr<ID3DBlob> pixelShader;
	Check(D3DCompileFromFile(
		L"Shaders/BoxPS.hlsl",
		nullptr,
		nullptr,
		"PS",
		"ps_5_0",
		compileFlags, 0, &pixelShader, nullptr));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	D3D12_INPUT_ELEMENT_DESC PipelineStateInputLayouts[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Set Input Layout
	psoDesc.InputLayout = { PipelineStateInputLayouts, _countof(PipelineStateInputLayouts)};

	psoDesc.pRootSignature = m_rootSignature.Get();

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	// Set RasterizerState
	//psoDesc.RasterizerState.FillMode = ;

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//psoDesc.DepthStencilState.DepthEnable = TRUE;
	//psoDesc.DepthStencilState.StencilEnable = FALSE;
	//psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	Check(Renderer->GetDevice()->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(m_pipelineState.GetAddressOf())
	));
}

void Ideal::Mesh::InitRootSignature2(std::shared_ptr<D3D12Renderer> Renderer)
{
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
	Check(Renderer->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.GetAddressOf())
	));
}

void Ideal::Mesh::InitPipelineState2(std::shared_ptr<D3D12Renderer> Renderer)
{
	ComPtr<ID3DBlob> errorBlob;
	uint32 compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// Set VS
	ComPtr<ID3DBlob> vertexShader;
	Check(D3DCompileFromFile(
		L"../Shaders/BoxUV.hlsl",
		nullptr,
		nullptr,
		"VS",
		"vs_5_0",
		compileFlags, 0, &vertexShader, &errorBlob));
	GetErrorBlob(errorBlob.Get());
	// Set PS
	ComPtr<ID3DBlob> pixelShader;
	Check(D3DCompileFromFile(
		L"../Shaders/BoxUV.hlsl",
		nullptr,
		nullptr,
		"PS",
		"ps_5_0",
		compileFlags, 0, &pixelShader, &errorBlob));
	GetErrorBlob(errorBlob.Get());
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	D3D12_INPUT_ELEMENT_DESC PipelineStateInputLayouts[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Set Input Layout
	psoDesc.InputLayout = { PipelineStateInputLayouts, _countof(PipelineStateInputLayouts) };

	psoDesc.pRootSignature = m_rootSignature.Get();

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// Set RasterizerState

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//psoDesc.DepthStencilState.DepthEnable = TRUE;
	//psoDesc.DepthStencilState.StencilEnable = FALSE;
	//psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	Check(Renderer->GetDevice()->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(m_pipelineState.GetAddressOf())
	));
}

void Ideal::Mesh::AddVertices(const std::vector<BasicVertex>& vertices)
{
	m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
}

void Ideal::Mesh::AddIndices(const std::vector<uint32>& indices)
{
	m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}
