#pragma once
#include "Core/Core.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"

namespace Ideal
{
	class D3D12VertexBuffer;
	class D3D12IndexBuffer;
	class D3D12UAVBuffer;
}

namespace Ideal
{
	struct DXRGeometryInfo
	{
		std::wstring Name;
		std::shared_ptr<Ideal::D3D12VertexBuffer> VertexBuffer;
		std::shared_ptr<Ideal::D3D12IndexBuffer> IndexBuffer;
	};

	class DXRAccelerationStructure
	{
	public:
		DXRAccelerationStructure() {};
		virtual ~DXRAccelerationStructure() {};

	public:
		ComPtr<ID3D12Resource> GetResource() { return m_accelerationStructure->GetResource(); }
		D3D12_GPU_VIRTUAL_ADDRESS const& GetGPUAddress() { return m_accelerationStructure->GetGPUVirtualAddress(); }

		uint64 RequiredScratchSize() { return max(m_preBuildInfo.ScratchDataSizeInBytes, m_preBuildInfo.UpdateScratchDataSizeInBytes); }
	protected:
		std::shared_ptr<Ideal::D3D12UAVBuffer> m_accelerationStructure;

		std::wstring m_name;
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS m_buildFlags;
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO m_preBuildInfo;

		bool m_isBuilt = false;	// 최소 한번이라도 빌드된 경우
		bool m_isDirty = true;	// as정보가 수정되어 재빌드가 필요한 경우
		bool m_updateOnBuild = false;
		bool m_allowUpdate = false;
	};

	class DXRBottomLevelAccelerationStructure : public DXRAccelerationStructure
	{
		static const uint32 MAX_PENDING_COUNT = SWAP_CHAIN_NUM - 1;

	public:
		DXRBottomLevelAccelerationStructure(const std::wstring& Name);
		virtual ~DXRBottomLevelAccelerationStructure();

	public:
		void Create(ComPtr<ID3D12Device5> Device,
			const DXRGeometryInfo& GeometryInfo,
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags,
			bool AllowUpdate
		);
		void Build(ComPtr<ID3D12GraphicsCommandList4> CommandList, ComPtr<ID3D12Resource> ScratchBuffer);

		// BLAS 안에 여러 Geometry가 있을 경우
		void AddGeometryInfo(const DXRGeometryInfo& GeometryInfo);

		uint32 GetInstanceContributionToHitGroupIndex() { return m_instanceContributionToHitGroupIndex; }
		void SetInstanceContributionToHitGroupIndex(uint32 Index) { m_instanceContributionToHitGroupIndex = Index; }

		bool IsDirty() { return m_isDirty; }
	private:
		void BuildGeometries();

	private:
		std::shared_ptr<Ideal::D3D12UAVBuffer> m_scratchBuffer;
		std::vector<DXRGeometryInfo> m_geometries;
		// geometry Info 로 만든 _geometry_desc을 저장 // 후에 BLAS 빌드할때 쓰임
		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> m_geometryDescs;

		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> m_cacheGeometryDescs[MAX_PENDING_COUNT];
		uint32 m_currentID = 0;	// cache Geomeetry Desc에 사용할 ID. 이전 프레임의 geometry Desc을 겹쳐쓰지 않기 위한 용도?

		uint32 m_instanceContributionToHitGroupIndex = 0;
	};

	class DXRTopLevelAccelerationStructure : public DXRAccelerationStructure
	{
	public:
		DXRTopLevelAccelerationStructure(const std::wstring& Name);
		virtual ~DXRTopLevelAccelerationStructure();

	public:
		void Create(
			ComPtr<ID3D12Device5> Device,
			uint32 NumBLASInstanceDescs,
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags,
			bool AllowUpdate = false
		);

		void Build(
			ComPtr<ID3D12GraphicsCommandList4> CommandList,
			uint32 NumInstanceDesc,
			D3D12_GPU_VIRTUAL_ADDRESS InstanceDescsGPUAddress,
			ComPtr<ID3D12Resource> ScratchBuffer
		);

	};


}

