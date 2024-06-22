#include "D3D12DescriptorManager.h"

Ideal::D3D12DescriptorManager::D3D12DescriptorManager()
{

}

Ideal::D3D12DescriptorManager::~D3D12DescriptorManager()
{

}

void Ideal::D3D12DescriptorManager::Create(ComPtr<ID3D12Device> Device, uint32 NumDescriptorPool, uint32 MaxCountDescriptorPool, uint32 NumFixedDescriptorHeap, uint32 MaxCountFixedDescriptorHeap)
{
	m_numDescriptorPool = NumDescriptorPool;
	m_numFixedDescriptorHeap = NumFixedDescriptorHeap;

	m_countDescriptorPool.resize(NumDescriptorPool);
	m_maxCountDescriptorPool = MaxCountDescriptorPool;
	m_countFixedDescriptorHeap.resize(NumFixedDescriptorHeap);
	m_maxCountDescriptorPool = MaxCountFixedDescriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	uint32 NumDescriptors = MaxCountDescriptorPool * NumDescriptorPool + MaxCountFixedDescriptorHeap * NumFixedDescriptorHeap;
	heapDesc.NumDescriptors = NumDescriptors;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	Check(
		Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap)),
		L"Failed to create Descriptor Manager"
	);
	m_descriptorSize = Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
}

void Ideal::D3D12DescriptorManager::ResetPool(uint32 PoolIndex)
{
	m_countDescriptorPool[PoolIndex] = 0;
}

void Ideal::D3D12DescriptorManager::ResetFixed(uint32 FixedDescriptorHeapIndex)
{
	m_countFixedDescriptorHeap[FixedDescriptorHeapIndex] = 0;
}

Ideal::D3D12DescriptorHandle Ideal::D3D12DescriptorManager::Allocate(uint32 DescriptorPoolIndex)
{
	uint32 maxCount = m_maxCountDescriptorPool;
	uint32 count = m_countDescriptorPool[DescriptorPoolIndex];
	if (count >= maxCount)
	{
		MyMessageBoxW(L"Can't Allocate Descriptor In DescriptorManager");
		__debugbreak();
		return Ideal::D3D12DescriptorHandle();
	}

	Ideal::D3D12DescriptorHandle ret = Ideal::D3D12DescriptorHandle(
		m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_descriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

	ret += DescriptorPoolIndex * maxCount * m_descriptorSize + count * m_descriptorSize;
	m_countDescriptorPool[DescriptorPoolIndex]++;

	return ret;
}

Ideal::D3D12DescriptorHandle Ideal::D3D12DescriptorManager::AllocateFixed(uint32 FixedDescriptorHeapIndex)
{
	uint32 maxCount = m_maxCountFixedDescriptorHeap;
	uint32 count = m_countFixedDescriptorHeap[FixedDescriptorHeapIndex];
	if (count >= maxCount)
	{
		MyMessageBoxW(L"Can't Allocate Descriptor In DescriptorManager");
		__debugbreak();
		return Ideal::D3D12DescriptorHandle();
	}

	Ideal::D3D12DescriptorHandle ret = Ideal::D3D12DescriptorHandle(
		m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_descriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

	ret += m_numDescriptorPool * m_maxCountDescriptorPool * m_descriptorSize;
	ret += FixedDescriptorHeapIndex * m_maxCountFixedDescriptorHeap * m_descriptorSize + count * m_descriptorSize;
	m_countFixedDescriptorHeap[FixedDescriptorHeapIndex]++;

	return ret;
}
