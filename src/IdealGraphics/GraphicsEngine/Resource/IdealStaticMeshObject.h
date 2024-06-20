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
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_transform = Transform; }
		virtual void SetDrawObject(bool IsDraw) override { m_isDraw = IsDraw; };

		const Matrix& GetTransformMatrix() const { return m_transform; }
		void SetStaticMesh(std::shared_ptr<Ideal::IdealStaticMesh> Mesh) { m_staticMesh = Mesh; }

	private:
		std::shared_ptr<IdealStaticMesh> m_staticMesh;
		Matrix m_transform;
		bool m_isDraw = true;

		//------Raytracing Info------//
	public:
		void AllocateBLASInstanceID(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::RaytracingManager> RaytracingManager);
		void UpdateBLASInstance(std::shared_ptr<Ideal::RaytracingManager> RaytracingManager);

	private:
		uint32 m_instanceID = 0;
	};
}