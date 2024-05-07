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
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_transform = Transform; }
		virtual void AddAnimation(const std::string& AnimationName, std::shared_ptr<Ideal::IAnimation> Animation) override;
		virtual void SetAnimation(const std::string& AnimationName, bool WhenCurrentAnimationFinished = true) override;

		//--TODO: ���߿� �������̽��� ���� ��--//
		uint32 GetCurrentAnimationIndex() { return m_currentFrame; };
		//----------------------------------//

		const Matrix& GetTransformMatrix() const { return m_transform; }
		void SetSkinnedMesh(std::shared_ptr<Ideal::IdealSkinnedMesh> Mesh);
		
		void AnimationPlay();

	private:
		std::shared_ptr<IdealSkinnedMesh> m_skinnedMesh;
		std::vector<std::shared_ptr<Ideal::IdealBone>> m_bones;

		Ideal::D3D12ConstantBuffer m_cbBone;
		CB_Bone m_cbBoneData;
		Ideal::D3D12ConstantBuffer m_cbTransform;
		CB_Transform m_cbTransformData;
		Matrix m_transform;

		// Animation
	private:
		/// Ver2
		std::map<std::string, std::shared_ptr<Ideal::IdealAnimation>> m_animations2;
		std::map<std::string, std::shared_ptr<AnimTransform>> m_animTransforms2;
		std::shared_ptr<Ideal::IdealAnimation> m_currentAnimation;
		std::shared_ptr<Ideal::IdealAnimation> m_nextAnimation;

		// ���� �ִϸ��̼��� ���� ��� �߰�
		bool m_whenCurrentAnimationFinishChangeAnimation = false;
		bool m_isAnimationFinished = false;

		float m_sumTime = 0.f;
		uint32 m_currentFrame = 0;
		uint32 m_nextFrame = 0;
		float m_animSpeed = 1.f;
		float m_ratio = 0.f;
	};
}

