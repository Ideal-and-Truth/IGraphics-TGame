#include "DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"

inline void AllocateUAVBuffer(ID3D12Device* pDevice, UINT64 bufferSize, ID3D12Resource** ppResource, D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, const wchar_t* resourceName = nullptr)
{
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	Check(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialResourceState,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName)
	{
		(*ppResource)->SetName(resourceName);
	}
}

Ideal::DXRBottomLevelAccelerationStructure::DXRBottomLevelAccelerationStructure(const std::wstring& Name)
{
	m_name = Name;
}

Ideal::DXRBottomLevelAccelerationStructure::~DXRBottomLevelAccelerationStructure()
{

}

void Ideal::DXRBottomLevelAccelerationStructure::Create(ComPtr<ID3D12Device5> Device, const DXRGeometryInfo& GeometryInfo, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool AllowUpdate)
{
	m_allowUpdate = AllowUpdate;
	std::shared_ptr<Ideal::D3D12VertexBuffer> VertexBuffer = GeometryInfo.VertexBuffer;
	std::shared_ptr<Ideal::D3D12IndexBuffer> IndexBuffer = GeometryInfo.IndexBuffer;

	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.Transform3x4 = 0;
	// index info
	geometryDesc.Triangles.IndexBuffer = IndexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexCount = IndexBuffer->GetElementCount();
	geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	// vertex info
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.VertexCount = VertexBuffer->GetElementCount();
	geometryDesc.Triangles.VertexBuffer.StartAddress = VertexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = VertexBuffer->GetElementSize();

	m_buildFlags = BuildFlags;
	if (AllowUpdate)
		m_buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPreBuildInfo = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = {};
	bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	bottomLevelInputs.Flags = m_buildFlags;
	bottomLevelInputs.NumDescs = 1;
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs= &geometryDesc;

	// Prebuild
	Device->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPreBuildInfo);
	Check(bottomLevelPreBuildInfo.ResultDataMaxSizeInBytes > 0);

	m_scratchBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
	m_scratchBuffer->Create(
		Device.Get(),
		bottomLevelPreBuildInfo.ScratchDataSizeInBytes,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	m_accelerationStructure = std::make_shared<Ideal::D3D12UAVBuffer>();
	m_accelerationStructure->Create(
		Device.Get(),
		bottomLevelPreBuildInfo.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		m_name.c_str()
	);
	// 최종 빌드
	/*D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	{
		bottomLevelBuildDesc.Inputs = bottomLevelInputs;
		bottomLevelBuildDesc.ScratchAccelerationStructureData = m_scratchBuffer->GetGPUVirtualAddress();
		bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	}
	CommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
	CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure->GetResource());
	CommandList->ResourceBarrier(1, &uavBarrier);*/
}

void Ideal::DXRBottomLevelAccelerationStructure::Build(ComPtr<ID3D12GraphicsCommandList4> CommandList)
{
	/*D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	{
		bottomLevelBuildDesc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		bottomLevelBuildDesc.Inputs.NumDescs = 1;
		bottomLevelBuildDesc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		bottomLevelBuildDesc.Inputs.Flags = m_buildFlags;
		if (m_isBuilt && m_allowUpdate && m_updateOnBuild)
		{
			bottomLevelBuildDesc.Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
			bottomLevelBuildDesc.SourceAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
		}
		bottomLevelBuildDesc.Inputs.NumDescs = 1;

	}*/
}

//------------------------TLAS------------------------//

Ideal::DXRTopLevelAccelerationStructure::DXRTopLevelAccelerationStructure(const std::wstring& Name)
{
	m_name = Name;
}

Ideal::DXRTopLevelAccelerationStructure::~DXRTopLevelAccelerationStructure()
{

}

void Ideal::DXRTopLevelAccelerationStructure::Create(ComPtr<ID3D12Device5> Device, uint32 NumInstanceDescs, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool AllowUpdate /*= false */)
{
	m_allowUpdate = AllowUpdate;
	m_buildFlags = BuildFlags;
	if (AllowUpdate)
	{
		m_buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	}

	// Prebuild Info
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	//topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY_OF_POINTERS;	// ppGeometryDescs << 여기에 Descs Pointer를 넘겨주면 될 듯
	topLevelInputs.Flags = m_buildFlags;
	topLevelInputs.NumDescs = NumInstanceDescs;
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPreBuildInfo = {};
	Device->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPreBuildInfo);
	Check(topLevelPreBuildInfo.ResultDataMaxSizeInBytes > 0);

	D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	m_accelerationStructure->Create(Device.Get(), topLevelPreBuildInfo.ResultDataMaxSizeInBytes, initialResourceState, m_name.c_str());
}
