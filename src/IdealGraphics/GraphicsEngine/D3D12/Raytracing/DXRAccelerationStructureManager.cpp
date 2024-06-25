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
	// ��Ű�� �������� ��� �׳� BLAS�� ���� �����.
	if (!IsSkinnedData)
	{
		// ���� ���� �̸��� BLAS�� �ִ����� �˻��Ѵ�. ���� ���� ��� �̹� �߰��� BLAS�� ��
		if (m_blasMap[Name] != nullptr)
		{
			//__debugbreak(); // �Ƹ� ���� �ɷ����� ���� �ڵ忡�� �̸����� ����� ã�ƿ��� ������ ���� ���̴�. 
			return m_blasMap[Name];
		}
	}

	// ó�� �߰��� ��� ���� �־��ش�.
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

	// �̸����� BLAS�� ��������
	std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = m_blasMap[BLASName];
	uint32 instanceIndex = AddInstanceByBLAS(blas, InstanceContributionToHitGroupIndex, transform, InstanceMask);
	return instanceIndex;
}

uint32 Ideal::DXRAccelerationStructureManager::AddInstanceByBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, uint32 InstanceContributionToHitGroupIndex /*= UINT_MAX*/, Matrix transform /*= Matrix::Identity*/, BYTE InstanceMask /*= 1*/)
{
	BLASInstanceDesc BLASInstance;
	BLASInstance.BLAS = BLAS;

	// ���� �ν��Ͻ��� �ε����� �����ϰ�
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
	// instanceDescs�� ù �ּҸ� �޾ƿ´�.
	D3D12_GPU_VIRTUAL_ADDRESS instanceDescs = 0;
	bool isFirst = true;

	// instance ������ UpdateBuffer�� �ø���.
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

		// ó���� �����ؾ��ϰų� blas ������ �����Ǿ� �ٽ� �����ؾ� �� ���
		if (ForceBuild || blas->IsDirty())
		{
			blas->Build(CommandList, m_scratchBuffer->GetResource());

			CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(blas->GetResource().Get());
			CommandList->ResourceBarrier(1, &uavBarrier);
		}
	}

	// Build TLAS
	{
		// TLAS�� �׻� �������Ѵ�..
		// ������ �޾ƿ� ù �ּҸ� ������
		m_topLevelAS->Build(CommandList, numInstance, instanceDescs, m_scratchBuffer->GetResource());

		CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_topLevelAS->GetResource().Get());
		CommandList->ResourceBarrier(1, &uavBarrier);
	}
}
