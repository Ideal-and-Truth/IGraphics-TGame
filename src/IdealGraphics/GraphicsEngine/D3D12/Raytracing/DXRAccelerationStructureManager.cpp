#include "DXRAccelerationStructureManager.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"

Ideal::DXRAccelerationStructureManager::DXRAccelerationStructureManager()
{

}

Ideal::DXRAccelerationStructureManager::~DXRAccelerationStructureManager()
{

}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::DXRAccelerationStructureManager::AddBLAS(ComPtr<ID3D12Device5> Device, std::vector<BLASGeometry>& Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, const wchar_t* Name, bool IsSkinnedData /*= false*/)
{
	// 스키닝 데이터일 경우 그냥 BLAS는 새로 만든다.
	if (!IsSkinnedData)
	{
		// 먼저 같은 이름의 BLAS가 있는지를 검사한다. 만약 있을 경우 이미 추가한 BLAS인 것
		if (m_blasMap[Name] != nullptr)
		{
			//__debugbreak(); // 아마 여기 걸렸으면 다음 코드에서 이름으로 제대로 찾아오면 문제는 없을 것이다. 
			return m_blasMap[Name];
		}
	}

	// 처음 추가할 경우 만들어서 넣어준다.
	std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = std::make_shared<Ideal::DXRBottomLevelAccelerationStructure>(Name);
	blas->Create(Device, Geometries, BuildFlags, false);
	m_blasMap[Name] = blas;

	if (blas->RequiredScratchSize() > m_scratchResourceSize)
	{
		m_scratchResourceSize = blas->RequiredScratchSize();
		if (!m_scratchBuffer)
		{
			m_scratchBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
		}
		m_scratchBuffer->Create(Device.Get(), m_scratchResourceSize, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ASScratchResource");
	}

	return blas;
}

uint32 Ideal::DXRAccelerationStructureManager::AddInstance(const std::wstring& BLASName, uint32 InstanceContributionToHitGroupIndex /*= UINT_MAX*/, Matrix transform /*= Matrix::Identity*/, BYTE InstanceMask /*= 1*/)
{
	BLASInstanceDesc BLASInstance;

	// 이름으로 BLAS를 꺼내오고
	std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = m_blasMap[BLASName];
	uint32 instanceIndex = AddInstanceByBLAS(blas, InstanceContributionToHitGroupIndex, transform, InstanceMask);
	return instanceIndex;
}

uint32 Ideal::DXRAccelerationStructureManager::AddInstanceByBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, uint32 InstanceContributionToHitGroupIndex /*= UINT_MAX*/, Matrix transform /*= Matrix::Identity*/, BYTE InstanceMask /*= 1*/)
{
	BLASInstanceDesc BLASInstance;
	BLASInstance.BLAS = BLAS;

	// 만들 인스턴스의 인덱스를 생성하고
	uint32 instanceIndex = m_currentBlasIndex;
	m_currentBlasIndex++;

	Ideal::DXRInstanceDesc instanceDesc = {};
	instanceDesc.InstanceMask = InstanceMask;
	instanceDesc.InstanceContributionToHitGroupIndex = InstanceContributionToHitGroupIndex;
	instanceDesc.AccelerationStructure = BLAS->GetGPUAddress();
	instanceDesc.SetTransform(transform);

	BLASInstance.InstanceDesc = instanceDesc;

	m_instanceDescs.push_back(BLASInstance);

	return instanceIndex;
}

void Ideal::DXRAccelerationStructureManager::InitTLAS(ComPtr<ID3D12Device5> Device, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool allowUpdate /*= false*/, const wchar_t* TLASName /*= nullptr*/)
{
	m_topLevelAS = std::make_shared<Ideal::DXRTopLevelAccelerationStructure>(TLASName);
	m_topLevelAS->Create(Device, m_currentBlasIndex, BuildFlags, allowUpdate);

	m_scratchBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
	m_scratchResourceSize = max(m_topLevelAS->RequiredScratchSize(), m_scratchResourceSize);
	m_scratchBuffer->Create(Device.Get(), m_scratchResourceSize, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ASScratchResource");
}

void Ideal::DXRAccelerationStructureManager::Build(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, bool ForceBuild /*= false*/)
{
	// instanceDescs의 첫 주소를 받아온다.
	D3D12_GPU_VIRTUAL_ADDRESS instanceDescs = 0;
	bool isFirst = true;

	// instance 정보를 UpdateBuffer에 올린다.
	uint32 numInstance = m_instanceDescs.size();

	for (uint32 i = 0; i < numInstance; ++i)
	{
		 std::shared_ptr<Ideal::UploadBufferContainer> container = UploadBufferPool->Allocate();
		 if (isFirst)
		 {
			 instanceDescs = container->GpuVirtualAddress;
			 isFirst = false;
		 }

		 DXRInstanceDesc* ptr = (DXRInstanceDesc*)container->SystemMemoryAddress;
		 *ptr = m_instanceDescs[i].InstanceDesc;
	}

	// Build BLAS
	for (auto& blasPair : m_blasMap)
	{
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = blasPair.second;

		// 처음에 빌드해야하거나 blas 정보가 수정되어 다시 빌드해야 할 경우
		if (ForceBuild || blas->IsDirty())
		{
			blas->Build(CommandList, m_scratchBuffer->GetResource());

			CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(blas->GetResource().Get());
			CommandList->ResourceBarrier(1, &uavBarrier);
		}
	}

	// Build TLAS
	{
		// TLAS는 항상 리빌드한다..
		// 위에서 받아온 첫 주소를 가져옴
		m_topLevelAS->Build(CommandList, numInstance, instanceDescs, m_scratchBuffer->GetResource());

		CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_topLevelAS->GetResource().Get());
		CommandList->ResourceBarrier(1, &uavBarrier);
	}
}
