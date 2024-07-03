#pragma once
#include "Core/Core.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

namespace Ideal
{
	class D3D12VertexBuffer;
	class D3D12IndexBuffer;
	class D3D12UAVBuffer;

	class DeferredDeleteManager;
}

namespace Ideal
{
	struct BLASGeometry
	{
		//BLASGeometry() : Name(L""), VertexBuffer(nullptr), IndexBuffer(nullptr), DiffuseTexture() {}
		BLASGeometry() : Name(L""), DiffuseTexture() {}
		
		std::wstring Name;
		Ideal::D3D12DescriptorHandle DiffuseTexture;
		//std::shared_ptr<Ideal::D3D12VertexBuffer> VertexBuffer;
		//std::shared_ptr<Ideal::D3D12IndexBuffer> IndexBuffer;
		//D3D12_GPU_DESCRIPTOR_HANDLE NormalTexture;

		ComPtr<ID3D12Resource> VertexBufferResource;
		D3D12_GPU_VIRTUAL_ADDRESS VertexBufferGPUAddress;
		uint32 VertexCount;
		uint32 VertexStrideInBytes;
		ComPtr<ID3D12Resource> IndexBufferResource;
		D3D12_GPU_VIRTUAL_ADDRESS IndexBufferGPUAddress;
		uint32 IndexCount;

		// refactor
		Ideal::D3D12DescriptorHandle SRV_VertexBuffer;
		Ideal::D3D12DescriptorHandle SRV_IndexBuffer;
		Ideal::D3D12DescriptorHandle SRV_Diffuse;
	};

	struct BLASData
	{
		std::vector<Ideal::BLASGeometry> Geometries;
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
		static const uint32 MAX_PENDING_COUNT = G_SWAP_CHAIN_NUM - 1;

	public:
		DXRBottomLevelAccelerationStructure(const std::wstring& Name);
		virtual ~DXRBottomLevelAccelerationStructure();

	public:
		void Create(ComPtr<ID3D12Device5> Device,
			std::vector<BLASGeometry>& Geometries,
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags,
			bool AllowUpdate
		);

		void Build(ComPtr<ID3D12GraphicsCommandList4> CommandList, ComPtr<ID3D12Resource> ScratchBuffer);

		uint32 GetInstanceContributionToHitGroupIndex() { return m_instanceContributionToHitGroupIndex; }
		void SetInstanceContributionToHitGroupIndex(uint32 Index) { m_instanceContributionToHitGroupIndex = Index; }

		uint64 GetGeometrySize() { return m_geometries.size(); }
		std::vector<BLASGeometry>& GetGeometries() { return m_geometries; }

		bool IsDirty() { return m_isDirty; }
		void SetDirty(bool Dirty) { m_isDirty = Dirty; }

	public:
		void BuildGeometries(std::vector<BLASGeometry>& Geometries);

	private:
		std::shared_ptr<Ideal::D3D12UAVBuffer> m_scratchBuffer;
		std::vector<BLASGeometry> m_geometries;
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
			std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager
		,
			bool AllowUpdate = false
		);
		void Create2(
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
		void Build2(
			ComPtr<ID3D12GraphicsCommandList4> CommandList,
			uint32 NumInstanceDesc,
			D3D12_GPU_VIRTUAL_ADDRESS InstanceDescsGPUAddress,
			ComPtr<ID3D12Resource> ScratchBuffer
		);
	};
}