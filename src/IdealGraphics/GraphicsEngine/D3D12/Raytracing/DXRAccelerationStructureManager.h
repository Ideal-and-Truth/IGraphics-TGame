#pragma once
#include "Core/Core.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/Raytracing/D3D12RaytracingResources.h"

using namespace DirectX;
namespace Ideal
{
	class D3D12VertexBuffer;
	class D3D12IndexBuffer;
	class D3D12UploadBufferPool;
}

namespace Ideal
{
	// �� �Ŵ����� �ϳ��� TLAS�� �������� BLAS, InstanceDesc�� �����Ѵ�.
	// �Ƹ� mesh���� ��� �ִ� �ֻ��� MeshObject�� �� �Ŵ����� �ϳ��� ������ �� �� ����.
	class DXRAccelerationStructureManager
	{
	public:
		DXRAccelerationStructureManager();
		~DXRAccelerationStructureManager();

	public:
		// BLAS�� ���ο��� ������ִ� ������ �ѱ��.
		void AddBLAS(ComPtr<ID3D12Device5> Device, std::vector<BLASGeometry>& Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, const wchar_t* Name);
		uint32 AddInstance(const std::wstring& BLASName, uint32 InstanceContributionToHitGroupIndex = UINT_MAX, Matrix transform = Matrix::Identity, BYTE InstanceMask = 1);
		DXRInstanceDesc* GetInstanceByIndex(uint32 InstanceIndex) { return &m_instanceDescs[InstanceIndex]; }
		void InitTLAS(ComPtr<ID3D12Device5> Device, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool allowUpdate = false, const wchar_t* TLASName = nullptr);
		void Build(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, bool ForceBuild = false);

		ComPtr<ID3D12Resource> GetTLASResource() { return m_topLevelAS->GetResource(); }
		const std::map<std::wstring, std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> GetBLASes() { return m_bottomLevelAS; }

	private:
		std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure> m_topLevelAS;
		std::map<std::wstring, std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> m_bottomLevelAS;

		std::shared_ptr<Ideal::D3D12UAVBuffer> m_scratchBuffer;
		uint64 m_scratchResourceSize = 0;
		uint32 m_currentBlasIndex = 0;
		std::vector<Ideal::DXRInstanceDesc> m_instanceDescs;
	};
}