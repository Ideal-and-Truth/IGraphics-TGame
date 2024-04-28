#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "GraphicsEngine/VertexInfo.h"

namespace Ideal
{
	template <typename>
	class IdealMesh;
	class IdealMaterial;
	class IdealRenderer;
	class IdealBone;
}

namespace Ideal
{
	class IdealSkinnedMeshRenderer : public ResourceBase
	{
	public:
		IdealSkinnedMeshRenderer();
		virtual ~IdealSkinnedMeshRenderer();

	public:
		void Render(std::shared_ptr<Ideal::IdealRenderer> Renderer);
		void SetTransformMatrix(const Matrix& Transform);

	public:
		void AddMeshes(std::shared_ptr<Ideal::IdealMesh<BasicVertex>> Mesh);
		void AddMaterial(std::shared_ptr<Ideal::IdealMaterial> Material);

	private:
		std::vector<std::shared_ptr<Ideal::IdealMesh<BasicVertex>>> m_meshes;
		std::vector<std::shared_ptr<Ideal::IdealBone>> m_bones;

		// TODO : Animation
		// std::vector<std::shared_ptr<Ideal::

		Matrix m_transform;;
	};
}