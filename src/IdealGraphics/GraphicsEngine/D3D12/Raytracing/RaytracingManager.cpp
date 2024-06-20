#include "RaytracingManager.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"
#include <d3d12.h>
#include <d3dx12.h>

Ideal::RaytracingManager::RaytracingManager()
{

}

Ideal::RaytracingManager::~RaytracingManager()
{

}

void Ideal::RaytracingManager::Init()
{
	m_ASManger = std::make_unique<DXRAccelerationStructureManager>();
}

uint32 Ideal::RaytracingManager::AddBLAS(ComPtr<ID3D12Device5> Device, Ideal::BLASGeometryDesc& GeometryDesc, const wchar_t* Name)
{
	// 예상으로는 중간에 만들어줘도 BLAS 자체에 IsDirty 초기값이 True 이기 때문에 Update할때 빌드가 될 것이다...
	m_ASManger->AddBLAS(Device.Get(), GeometryDesc, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, Name);
	uint32 instanceIndex = m_ASManger->AddInstance(Name, 1);
	return instanceIndex;
}

void Ideal::RaytracingManager::SetGeometryTransformByIndex(uint32 InstanceIndex, const Matrix& Transform)
{
	Ideal::DXRInstanceDesc* instance = m_ASManger->GetInstanceByIndex(InstanceIndex);
	instance->SetTransform(Transform);
}

void Ideal::RaytracingManager::FinalCreate(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool)
{
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	m_ASManger->InitTLAS(Device.Get(), buildFlags, true, L"RaytracingManager TLAS");
	m_ASManger->Build(CommandList, UploadBufferPool, true);
}

void Ideal::RaytracingManager::UpdateAccelerationStructures(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool)
{
	m_ASManger->Build(CommandList, UploadBufferPool);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Ideal::RaytracingManager::GetTLASResource()
{
	return m_ASManger->GetTLASResource();
}
