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
	// Transform 이 필요 없을지도
	class IdealStaticMesh : public ResourceBase
	{
	public:
		IdealStaticMesh();
		virtual ~IdealStaticMesh();

	public:
		void Render(std::shared_ptr<Ideal::IdealRenderer> Renderer);
		void SetTransformMatrix(const Matrix& Transform);

	public:
		void AddMesh(std::shared_ptr<Ideal::IdealMesh<BasicVertex>> Mesh);
		void AddMaterial(std::shared_ptr<Ideal::IdealMaterial> Material);
		void AddBone(std::shared_ptr<Ideal::IdealBone> Bone) { m_bones.push_back(Bone); }

		void FinalCreate(std::shared_ptr<Ideal::IdealRenderer> Renderer);

	private:
		std::vector<std::shared_ptr<Ideal::IdealMesh<BasicVertex>>> m_meshes;
		Matrix m_transform;
		std::vector<std::shared_ptr<Ideal::IdealBone>> m_bones;
	};
}