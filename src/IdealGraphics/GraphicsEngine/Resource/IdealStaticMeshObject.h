#pragma once
#include "GraphicsEngine/public/IMeshObject.h"
#include "Core/Core.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/Resource/IdealStaticMesh.h"

namespace Ideal
{
	class IdealRenderer;
	class IdealStaticMesh;
	class D3D12Renderer;

	//--raytracing--//
	class RaytracingManager;
	class DXRBottomLevelAccelerationStructure;
	class BLASInstanceDesc;
}


namespace Ideal
{
	class IdealStaticMeshObject : public IMeshObject
	{
	public:
		IdealStaticMeshObject();
		virtual ~IdealStaticMeshObject();
	public:
		// Interface //
		virtual uint32 GetMeshesSize() override;
		virtual std::weak_ptr<Ideal::IMesh> GetMeshByIndex(uint32 index) override;
		virtual uint32 GetBonesSize() override;
		virtual std::weak_ptr<Ideal::IBone> GetBoneByIndex(uint32 index) override;

	public:
		void Init(ComPtr<ID3D12Device> Device);
		// 2024.05.07 Ver2
		void Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer);

	public:
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_transform = Transform; m_isDirty = true; }
		virtual void SetDrawObject(bool IsDraw) override { m_isDraw = IsDraw; };

		virtual Matrix GetLocalTransformMatrix(){ return m_staticMesh->GetLocalTM(); };

		virtual Ideal::EMeshType GetMeshType() const override { return EMeshType::Static; }

		const Matrix& GetTransformMatrix() const { return m_transform; }
		void SetStaticMesh(std::shared_ptr<Ideal::IdealStaticMesh> Mesh) { m_staticMesh = Mesh; }
		std::shared_ptr<Ideal::IdealStaticMesh> GetStaticMesh() { return m_staticMesh; }
	private:
		std::shared_ptr<IdealStaticMesh> m_staticMesh;
		Matrix m_transform;
		bool m_isDraw = true;

		//------Raytracing Info------//
	public:
		void UpdateBLASInstance(std::shared_ptr<Ideal::RaytracingManager> RaytracingManager);
		void SetBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> InBLAS);
		void SetBLASInstanceIndex(uint32 InstanceID) { m_instanceIndex = InstanceID; }
		void SetBLASInstanceDesc(std::shared_ptr<Ideal::BLASInstanceDesc> InstanceDesc) { m_BLASInstanceDesc = InstanceDesc; }
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> GetBLAS();
		std::shared_ptr<Ideal::BLASInstanceDesc> GetBLASInstanceDesc() { return m_BLASInstanceDesc; }

	private:
		bool m_isDirty = false;
		uint32 m_instanceIndex = 0;
		std::shared_ptr<Ideal::BLASInstanceDesc> m_BLASInstanceDesc;
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> m_blas;
	};
}