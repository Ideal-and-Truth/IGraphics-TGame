#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/VertexInfo.h"

namespace Ideal
{
	template <typename> class IdealMesh;
	class IdealMaterial;
	class IdealRenderer;
	class IdealBone;
}

namespace Ideal
{
	class IdealDynamicMesh : public ResourceBase
	{
	public:
		IdealDynamicMesh();
		virtual ~IdealDynamicMesh();

	public:
		void Render(std::shared_ptr<Ideal::IdealRenderer> Renderer);
		void SetTransformMatrix(const Matrix& Transform);

	public:
		void AddMesh(std::shared_ptr<Ideal::IdealMesh<SkinnedVertex>> Mesh);
		void AddMaterial(std::shared_ptr<Ideal::IdealMaterial> Material);
		void AddBone(std::shared_ptr<Ideal::IdealBone> Bone) { m_bones.push_back(Bone); }

		void FinalCreate(std::shared_ptr<Ideal::IdealRenderer> Renderer);

	private:
		std::vector<std::shared_ptr<Ideal::IdealMesh<SkinnedVertex>>> m_meshes;
		Matrix m_transform;
		Ideal::D3D12ConstantBuffer m_constantBuffer;
		std::vector<std::shared_ptr<Ideal::IdealBone>> m_bones;
	};
}