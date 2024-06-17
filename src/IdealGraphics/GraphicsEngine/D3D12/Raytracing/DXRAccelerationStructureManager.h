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
	// 이 매니저는 하나의 TLAS와 여러개의 BLAS, InstanceDesc을 관리한다.
	// 아마 mesh들을 들고 있는 최상위 MeshObject가 이 매니저를 하나씩 가지면 될 것 같다.
	class DXRAccelerationStructureManager
	{
	public:
		DXRAccelerationStructureManager();
		~DXRAccelerationStructureManager();

	public:
		// BLAS는 내부에서 만들어주니 정보를 넘긴다.
		void AddBLAS(ComPtr<ID3D12Device5> Device, const DXRGeometryInfo& GeometryInfo, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags);
		uint32 AddInstance(const std::wstring& BLASName, uint32 InstanceContributionToHitGroupIndex = UINT_MAX, Matrix transform = Matrix::Identity, BYTE InstanceMask = 1);
		void InitTLAS(ComPtr<ID3D12Device5> Device, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool allowUpdate = false, const wchar_t* TLASName = nullptr);
		void Build(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, bool ForceBuild = false);

	private:
		std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure> m_topLevelAS;
		std::map<std::wstring, std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> m_bottomLevelAS;

		std::shared_ptr<Ideal::D3D12UAVBuffer> m_scratchBuffer;
		uint64 m_scratchResourceSize = 0;
		uint32 m_currentBlasIndex = 0;
		std::vector<Ideal::DXRInstanceDesc> m_instanceDescs;
	};
}