#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"


struct ID3D12Device5;
struct ID3D12GraphicsCommandList4;

namespace Ideal
{
	class DXRAccelerationStructureManager;
	class D3D12UploadBufferPool;
}

namespace Ideal
{
	/// <summary>
	/// 해야 할 기능 정리
	/// Mesh Object를 만들때마다 BLAS를 생성해야 한다.
	///		-> BLAS 생성 및 TLAS 리빌드
	/// Shader Table 관리도 해야할 것 같음.
	/// BLAS TLAS를 재빌드 또는 업데이트를 해야한다.
	/// 
	/// </summary>
	class RaytracingManager
	{
	public:
		RaytracingManager();
		~RaytracingManager();

	public:
		void Init();
		uint32 AddBLAS(ComPtr<ID3D12Device5> Device, Ideal::BLASGeometryDesc& GeometryDesc, const wchar_t* Name);
		void SetGeometryTransformByIndex(uint32 InstanceIndex, const Matrix& Transform);
		void FinalCreate(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool);
		void UpdateAccelerationStructures(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool);
		ComPtr<ID3D12Resource> GetTLASResource();

	private:
		std::unique_ptr<DXRAccelerationStructureManager> m_ASManger;

	};
}
