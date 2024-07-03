#include "DXRAccelerationStructureManager.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"

#include "GraphicsEngine/D3D12/D3D12RayTracingRenderer.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"

//#define OneScratchBuffer

Ideal::DXRAccelerationStructureManager::DXRAccelerationStructureManager()
{

}

Ideal::DXRAccelerationStructureManager::~DXRAccelerationStructureManager()
{

}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::DXRAccelerationStructureManager::AddBLAS2(std::shared_ptr<Ideal::D3D12RayTracingRenderer> Renderer, ComPtr<ID3D12Device5> Device, std::vector<BLASGeometry>& Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool AllowUpdate, const wchar_t* Name, bool IsSkinnedData /*= false*/)
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
	blas->Create(Device, Geometries, BuildFlags, AllowUpdate);
	m_blasMap[Name] = blas;

	if (blas->RequiredScratchSize() > m_scratchResourceSize)
	{
		// wait�� �ɾ������ scratch buffer�� �ٽ� ������ �Ѵ�.
		Renderer->Fence();
		for (DWORD i = 0; i < Ideal::D3D12RayTracingRenderer::MAX_PENDING_FRAME_COUNT; i++)
		{
			Renderer->WaitForFenceValue(Renderer->m_lastFenceValues[i]);
		}

		m_scratchResourceSize = blas->RequiredScratchSize();

		// ver2
		for (uint32 i = 0; i < MAX_PENDING_FRAME; ++i)
		{
			m_scratchBuffers[i] = std::make_shared<Ideal::D3D12UAVBuffer>();
			m_scratchBuffers[i]->Create(Device.Get(), m_scratchResourceSize, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ASStructures2");
		}
	}
	m_blasVector.push_back(blas);
	return blas;
}

uint32 Ideal::DXRAccelerationStructureManager::AddInstanceByBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, uint32 InstanceContributionToHitGroupIndex /*= UINT_MAX*/, Matrix transform /*= Matrix::Identity*/, BYTE InstanceMask /*= 1*/)
{
	BLASInstanceDesc BLASInstance;
	BLASInstance.BLAS = BLAS;

	// ���� �ν��Ͻ��� �ε����� �����ϰ�
	uint32 instanceIndex = m_currentBlasInstanceIndex;
	m_currentBlasInstanceIndex++;

	Ideal::DXRInstanceDesc instanceDesc = {};
	instanceDesc.InstanceMask = InstanceMask;

	if (InstanceContributionToHitGroupIndex == UINT_MAX)
		InstanceContributionToHitGroupIndex = BLAS->GetInstanceContributionToHitGroupIndex();

	instanceDesc.InstanceContributionToHitGroupIndex = InstanceContributionToHitGroupIndex;
	instanceDesc.AccelerationStructure = BLAS->GetGPUAddress();
	instanceDesc.SetTransform(transform);

	BLASInstance.InstanceDesc = instanceDesc;

	m_instanceDescs.push_back(BLASInstance);

	return instanceIndex;
}

void Ideal::DXRAccelerationStructureManager::InitTLAS2(ComPtr<ID3D12Device5> Device, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool allowUpdate /*= false*/, const wchar_t* TLASName /*= nullptr*/)
{
	for (uint32 i = 0; i < MAX_PENDING_FRAME; ++i)
	{
		m_topLevelASs[i] = std::make_shared<Ideal::DXRTopLevelAccelerationStructure>(TLASName);
		m_topLevelASs[i]->Create(Device, m_currentBlasInstanceIndex, BuildFlags, nullptr, allowUpdate);
	}

	m_scratchResourceSize = max(m_topLevelASs[0]->RequiredScratchSize(), m_scratchResourceSize);

	for (uint32 i = 0; i < MAX_PENDING_FRAME; ++i)
	{
		m_scratchBuffers[i] = std::make_shared<Ideal::D3D12UAVBuffer>();
		m_scratchBuffers[i]->Create(Device.Get(), m_scratchResourceSize, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ASStructures2");
	}
}

void Ideal::DXRAccelerationStructureManager::Build2(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager, bool ForceBuild /*= false*/)
{
	UploadBufferPool->Reset();
	m_currentIndex = (m_currentIndex + 1) % MAX_PENDING_FRAME;

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
	for (auto& blas : m_blasVector)
	{
		// ó���� �����ؾ��ϰų� blas ������ �����Ǿ� �ٽ� �����ؾ� �� ���
		if (ForceBuild || blas->IsDirty())
		{
			// ver2
			blas->Build(CommandList, m_scratchBuffers[m_currentIndex]->GetResource());
			CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(blas->GetResource().Get());
			CommandList->ResourceBarrier(1, &uavBarrier);
		}
	}

	// Build TLAS
	{
			m_topLevelASs[m_currentIndex]->Create(Device, m_currentBlasInstanceIndex, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, nullptr, false);
		m_topLevelASs[m_currentIndex]->Build(CommandList, numInstance, instanceDescs, m_scratchBuffers[m_currentIndex]->GetResource());

		CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_topLevelASs[m_currentIndex]->GetResource().Get());
		CommandList->ResourceBarrier(1, &uavBarrier);
	}
}