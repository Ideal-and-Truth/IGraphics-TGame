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
}


namespace Ideal
{
	class IdealStaticMeshObject : public IMeshObject
	{
	public:
		IdealStaticMeshObject();
		virtual ~IdealStaticMeshObject();

		void Init(ComPtr<ID3D12Device> Device);
		// 2024.05.07 Ver2
		void Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer);

	public:
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_transform = Transform; m_isDirty = true; }
		virtual void SetDrawObject(bool IsDraw) override { m_isDraw = IsDraw; };

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

	private:
		bool m_isDirty = false;
		uint32 m_instanceIndex = 0;
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> m_blas;
	};
}