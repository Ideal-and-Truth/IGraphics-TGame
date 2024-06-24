#pragma once
#include "GraphicsEngine/public/ISkinnedMeshObject.h"
#include "GraphicsEngine/ConstantBufferInfo.h"
#include "GraphicsEngine/Resource/IdealBone.h"

namespace Ideal
{
	class IAnimation;

	class Idealbone;
	class IdealRenderer;
	class IdealAnimation;
	class IdealSkinnedMesh;
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
		virtual void SetDrawObject(bool IsDraw) override { m_isDraw = IsDraw; };
		virtual void AddAnimation(const std::string& AnimationName, std::shared_ptr<Ideal::IAnimation> Animation) override;
		virtual void SetAnimation(const std::string& AnimationName, bool WhenCurrentAnimationFinished = true) override;

		//--TODO: 나중에 인터페이스로 뽑을 것--//
		virtual uint32 GetCurrentAnimationIndex() override { return m_currentFrame; };
		//----------------------------------//

		const Matrix& GetTransformMatrix() const { return m_transform; }
		void SetSkinnedMesh(std::shared_ptr<Ideal::IdealSkinnedMesh> Mesh);
		
		void AnimationPlay();

	private:
		bool m_isDraw = true;

		std::shared_ptr<Ideal::IdealSkinnedMesh> m_skinnedMesh;
		std::vector<std::shared_ptr<Ideal::IdealBone>> m_bones;

		CB_Bone m_cbBoneData;
		CB_Transform m_cbTransformData;

		Matrix m_transform;

		// Animation
	private:
		/// Ver2
		std::map<std::string, std::shared_ptr<Ideal::IdealAnimation>> m_animations;
		std::map<std::string, std::shared_ptr<AnimTransform>> m_animTransforms;
		std::shared_ptr<Ideal::IdealAnimation> m_currentAnimation;
		std::shared_ptr<Ideal::IdealAnimation> m_nextAnimation;

		// 다음 애니메이션이 있을 경우 추가
		bool m_whenCurrentAnimationFinishChangeAnimation = false;
		bool m_isAnimationFinished = false;

		float m_sumTime = 0.f;
		uint32 m_currentFrame = 0;
		uint32 m_nextFrame = 0;
		float m_animSpeed = 1.f;
		float m_ratio = 0.f;
	};
}

