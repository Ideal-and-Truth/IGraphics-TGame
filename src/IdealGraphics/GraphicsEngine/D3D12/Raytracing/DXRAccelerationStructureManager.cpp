#include "DXRAccelerationStructureManager.h"

Ideal::DXRAccelerationStructureManager::DXRAccelerationStructureManager()
{

}

Ideal::DXRAccelerationStructureManager::~DXRAccelerationStructureManager()
{

}

void Ideal::DXRAccelerationStructureManager::AddBLAS(ComPtr<ID3D12Device5> Device, const DXRGeometryInfo& GeometryInfo, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags)
{
	std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = std::make_shared<Ideal::DXRBottomLevelAccelerationStructure>();
	blas->Create(Device, GeometryInfo, BuildFlags, false);

	m_bottomLevelAS.push_back(blas);

	// instance
	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
	instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1; // identity
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.AccelerationStructure = blas->GetGPUAddress();

	//// upload buffer
	//Ideal::D3D12UploadBuffer uploadBuffer;
	//uploadBuffer.Create(Device, )
}

uint32 Ideal::DXRAccelerationStructureManager::AddInstance(const std::wstring& BLASName, uint32 InstanceContributionToHitGroupIndex /*= UINT_MAX*/, Matrix transform /*= Matrix::Identity*/, BYTE InstanceMask /*= 1*/)
{
	//uint32 instanceIndex = m_numBLASInstance;
	//m_numBLASInstance++;

	//D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
	//instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1; // identity
	//instanceDesc.InstanceMask = InstanceMask;
	////instanceDesc.InstanceContributionToHitGroupIndex = InstanceContributionToHitGroupIndex != UINT_MAX ? InstanceContributionToHitGroupIndex :  
	//instanceDesc.AccelerationStructure = 
}

void Ideal::DXRAccelerationStructureManager::InitTLAS(ComPtr<ID3D12Device5> Device, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool allowUpdate /*= false*/)
{
	m_topLevelAS = std::make_shared<Ideal::DXRTopLevelAccelerationStructure>();
	//m_topLevelAS->Create(Device,)

	uint32 blasNum = (uint32)m_bottomLevelAS.size();

	Ideal::D3D12UploadBuffer instanceBufferDest;

	instanceBufferDest.Create(Device.Get(), sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * blasNum);
	void* data = instanceBufferDest.Map();
	
	// TODO : instance gpu address
}
