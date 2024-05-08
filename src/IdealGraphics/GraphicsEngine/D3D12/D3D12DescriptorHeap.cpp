#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"

#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"

using namespace Ideal;


//------------------Descriptor Handle-------------------//

D3D12DescriptorHandle::D3D12DescriptorHandle()
{
	//m_cpuHandle = (D3D12_GPU_VIRTUAL_ADDRESS)(-1);
}

D3D12DescriptorHandle::D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle)
	: m_cpuHandle(CpuHandle),
	m_gpuHandle(GpuHandle)
{

}

D3D12DescriptorHandle::D3D12DescriptorHandle(const D3D12DescriptorHandle& Other)
{
	m_cpuHandle = Other.m_cpuHandle;
	m_gpuHandle = Other.m_gpuHandle;
}

D3D12DescriptorHandle::~D3D12DescriptorHandle()
{

}

//------------------Descriptor Heaap-------------------//

D3D12DescriptorHeap::D3D12DescriptorHeap()
	: m_numFreeDescriptors(0),
	m_descriptorSize(0)
{

}

D3D12DescriptorHeap::~D3D12DescriptorHeap()
{

}

void D3D12DescriptorHeap::Create(std::shared_ptr<D3D12Renderer> Renderer, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, uint32 MaxCount)
{
	m_maxCount = MaxCount;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = MaxCount;
	heapDesc.Type = HeapType;
	heapDesc.Flags = Flags;
	Check(Renderer->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));

	m_descriptorSize = Renderer->GetDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);
	m_numFreeDescriptors = heapDesc.NumDescriptors;

	// 첫 위치로 Handle을 만든다.
	m_freeHandle = Ideal::D3D12DescriptorHandle(
		m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_descriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

	m_headHandle = m_freeHandle;
}

void D3D12DescriptorHeap::Create(ID3D12Device* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, uint32 MaxCount)
{
	m_maxCount = MaxCount;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = MaxCount;
	heapDesc.Type = HeapType;
	heapDesc.Flags = Flags;
	Check(Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));

	m_descriptorSize = Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
	m_numFreeDescriptors = heapDesc.NumDescriptors;

	// 첫 위치로 Handle을 만든다.
	m_freeHandle = Ideal::D3D12DescriptorHandle(
		m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_descriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

	m_headHandle = m_freeHandle;
}

Ideal::D3D12DescriptorHandle D3D12DescriptorHeap::Allocate(uint32 Count /* = 1*/)
{
	if (Count == 0)
	{
		assert(false);
		MessageBox(NULL, L"Cant' Allocate Descriptor", L"오류", MB_OK);
		return Ideal::D3D12DescriptorHandle();
	}
	// 할당할 수 있는 개수를 넘어섰을 경우
	if (m_numFreeDescriptors < Count)
	{
		assert(false);
		MessageBox(NULL, L"Cant' Allocate Descriptor", L"오류", MB_OK);
		return Ideal::D3D12DescriptorHandle();
	}

	// return 값은 FreeHandle의 주소를 이동시키기 전 , 즉 현재 비어있는 descriptor heap의 주소를 가지고 만든다.
	Ideal::D3D12DescriptorHandle ret = m_freeHandle;
	m_freeHandle += Count * m_descriptorSize;
	m_numFreeDescriptors -= Count;

	return ret;
}

void D3D12DescriptorHeap::Reset()
{
	m_count = 0;
	m_numFreeDescriptors = m_maxCount;
	m_freeHandle = m_headHandle;
}
