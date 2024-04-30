#pragma once
#include "GraphicsEngine/public/ISkinnedMeshObject.h"
#include "Core/Core.h"

#include "GraphicsEngine/VertexInfo.h"

#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/Resource/Refactor/IdealSkinnedMesh.h"

namespace Ideal
{
	class IAnimation;

	class IdealRenderer;
	class IdealAnimation;
}
struct AnimTransform
{
	using TransformArrayType = std::array<Matrix, MAX_BONE_TRANSFORMS>;
	std::array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms;
};

namespace Ideal
{
	class IdealSkinnedMeshObject : public ISkinnedMeshObject
	{
	public:
		IdealSkinnedMeshObject();
		virtual ~IdealSkinnedMeshObject();

		void Init(std::shared_ptr<IdealRenderer> Renderer);
		void Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer);

	public:
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_dynamicMesh->SetTransformMatrix(Transform); }
		virtual void AddAnimation(std::shared_ptr<Ideal::IAnimation> Animation) override;

		const Matrix& GetTransformMatrix() const { return m_transform; }
		void SetDynamicMesh(std::shared_ptr<Ideal::IdealSkinnedMesh> Mesh) { 
			m_dynamicMesh = Mesh;
			SetBone(Mesh->GetBones()); }

		void SetBone(std::vector<std::shared_ptr<Ideal::IdealBone>>& Bones) { m_bones = Bones; };

		// Test : 임시로 한 애니메이션을 조립해서 띄워보겠음
		void CreateAnimationTransform();

	private:
		std::shared_ptr<IdealSkinnedMesh> m_dynamicMesh;
		
		Matrix m_transform;

		std::vector<std::shared_ptr<Ideal::IdealAnimation>> m_animations;
		std::vector<std::shared_ptr<Ideal::IdealBone>> m_bones;

		std::vector<std::shared_ptr<AnimTransform>> m_animTransforms;

		Ideal::D3D12ConstantBuffer m_cbBone;
		CB_Bone m_cbBoneData;
	};
}

