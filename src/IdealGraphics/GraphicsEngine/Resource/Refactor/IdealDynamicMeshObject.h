#pragma once
#include "GraphicsEngine/public/IDynamicMeshObject.h"
#include "Core/Core.h"

#include "GraphicsEngine/VertexInfo.h"

#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/Resource/Refactor/IdealDynamicMesh.h"
namespace Ideal
{
	class IAnimation;

	class IdealRenderer;
	class IdealAnimation;
}

namespace Ideal
{
	class IdealDynamicMeshObject : public IDynamicMeshObject
	{
	public:
		IdealDynamicMeshObject();
		virtual ~IdealDynamicMeshObject();

		void Init(std::shared_ptr<IdealRenderer> Renderer);
		void Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer);

	public:
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_dynamicMesh->SetTransformMatrix(Transform); }
		virtual void AddAnimation(std::shared_ptr<Ideal::IAnimation> Animation) override;

		const Matrix& GetTransformMatrix() const { return m_transform; }
		void SetDynamicMesh(std::shared_ptr<Ideal::IdealDynamicMesh> Mesh) { m_dynamicMesh = Mesh; }

	private:
		std::shared_ptr<IdealDynamicMesh> m_dynamicMesh;
		
		Matrix m_transform;

		std::vector<std::shared_ptr<Ideal::IdealAnimation>> m_animations;
	};
}

