#pragma once
#include "GraphicsEngine/public/IMeshObject.h"
#include "Core/Core.h"
#include "GraphicsEngine/VertexInfo.h"

namespace Ideal
{
	class IdealRenderer;
	class IdealStaticMesh;
}

namespace Ideal
{
	class IdealStaticMeshObject : public IMeshObject
	{
	public:
		IdealStaticMeshObject();
		virtual ~IdealStaticMeshObject();

		void Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer);

	public:
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_transform = Transform; }
		const Matrix& GetTransformMatrix() const { return m_transform; }
		void SetStaticMesh(std::shared_ptr<Ideal::IdealStaticMesh> Mesh) { m_staticMesh = Mesh; }

	private:
		Matrix m_transform;
		std::shared_ptr<IdealStaticMesh> m_staticMesh;
	};
}