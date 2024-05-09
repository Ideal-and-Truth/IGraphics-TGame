#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"

namespace Ideal
{
	template <typename> class IdealMesh;
	class IdealMaterial;
	class IdealRenderer;
	class IdealBone;
}

struct SkinnedVertex;

namespace Ideal
{
	class IdealSkinnedMesh : public ResourceBase
	{
	public:
		IdealSkinnedMesh();
		virtual ~IdealSkinnedMesh();

	public:
		void Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer);
	public:
		void AddMesh(std::shared_ptr<Ideal::IdealMesh<SkinnedVertex>> Mesh);
		void AddMaterial(std::shared_ptr<Ideal::IdealMaterial> Material);
		void AddBone(std::shared_ptr<Ideal::IdealBone> Bone) { m_bones.push_back(Bone); }

		void FinalCreate(std::shared_ptr<Ideal::IdealRenderer> Renderer);

		std::vector<std::shared_ptr<Ideal::IdealBone>>& GetBones() { return m_bones; }

	private:
		std::vector<std::shared_ptr<Ideal::IdealMesh<SkinnedVertex>>> m_meshes;
		std::vector<std::shared_ptr<Ideal::IdealBone>> m_bones;
	};
}