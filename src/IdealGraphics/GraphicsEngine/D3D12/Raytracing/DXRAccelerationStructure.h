#pragma once
#include "Core/Core.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12Resource.h"

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

	protected:
		std::shared_ptr<Ideal::D3D12UAVBuffer> m_accelerationStructure;

		std::wstring m_name;
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS m_buildFlags;
		bool m_allowUpdate = false;
	};

	class DXRBottomLevelAccelerationStructure : public DXRAccelerationStructure
	{
	public:
		DXRBottomLevelAccelerationStructure(const std::wstring& Name);
		virtual ~DXRBottomLevelAccelerationStructure();

	public:
		void Create(ComPtr<ID3D12Device5> Device,
			const DXRGeometryInfo& GeometryInfo,
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags,
			bool AllowUpdate
		);
		void Build(ComPtr<ID3D12GraphicsCommandList4> CommandList);

		uint32 GetInstanceContributionToHitGroupIndex() { return m_instanceContributionToHitGroupIndex; }
		void SetInstanceContributionToHitGroupIndex(uint32 Index) { m_instanceContributionToHitGroupIndex = Index; }

	private:
		std::shared_ptr<Ideal::D3D12UAVBuffer> m_scratchBuffer;

		uint32 m_instanceContributionToHitGroupIndex = 0;

		bool m_isBuilt = false;	// 최소 한번이라도 빌드된 경우
		bool m_isDirty = true;	// as정보가 수정되어 재빌드가 필요한 경우
		bool m_updateOnBuild = false;
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
	};


}

