#pragma once
#include "GraphicsEngine/public/ISkinnedMeshObject.h"
#include "GraphicsEngine/ConstantBufferInfo.h"
#include "GraphicsEngine/Resource/IdealBone.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"

#include <d3d12.h>

namespace Ideal
{
	class IAnimation;

	class Idealbone;
	class IdealRenderer;
	class IdealAnimation;
	class IdealSkinnedMesh;
	class RaytracingManager;
	class DXRBottomLevelAccelerationStructure;
	class ResourceManager;
	class D3D12DynamicConstantBufferAllocator;
	class D3D12UAVBuffer;
	class D3D12DescriptorManager;
	class D3D12UnorderedAccessView;
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
		virtual void SetTransformMatrix(const Matrix& Transform) override { m_transform = Transform; m_isDirty = true; }
		virtual void SetDrawObject(bool IsDraw) override { m_isDraw = IsDraw; };
		virtual void AddAnimation(const std::string& AnimationName, std::shared_ptr<Ideal::IAnimation> Animation) override;
		virtual void SetAnimation(const std::string& AnimationName, bool WhenCurrentAnimationFinished = true) override;

		//--TODO: 나중에 인터페이스로 뽑을 것--//
		uint32 GetCurrentAnimationIndex() { return m_currentFrame; };
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

		//------Raytracing Info------//
	public:
		void CreateUAVVertexBuffer(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager);
		std::shared_ptr<Ideal::IdealSkinnedMesh> GetSkinnedMesh() { return m_skinnedMesh; }
		void AllocateBLASInstanceID(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::RaytracingManager> RaytracingManager, std::shared_ptr<Ideal::ResourceManager> ResourceManager);
		void UpdateBLASInstance(
			ComPtr<ID3D12Device5> Device,
			ComPtr<ID3D12GraphicsCommandList4> CommandList,
			std::shared_ptr<Ideal::ResourceManager> ResourceManager,
			std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager,
			uint32 CurrentContextIndex,
			std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool,
			std::shared_ptr<Ideal::RaytracingManager> RaytracingManager
		);
		void SetBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> InBLAS);
		//std::vector<BLASGeometry> GetBLASGeometries(ComPtr<ID3D12Device> Device, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager);
		void SetBLASInstanceIndex(uint32 InstanceIndex);
		std::shared_ptr<Ideal::D3D12UAVBuffer> GetUAV_VertexBuffer() { return m_uavBuffer; }

	private:
		uint32 m_instanceIndex = 0;
		bool m_isDirty = false;
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> m_BLAS;

		//static const uint32 MAX_PENDING_COUNT = G_SWAP_CHAIN_NUM - 1;
		//uint32 m_currentID = 0;
		//std::shared_ptr<Ideal::D3D12UAVBuffer> m_uavBuffer[MAX_PENDING_COUNT];
		//std::shared_ptr<Ideal::D3D12UnorderedAccessView> m_uavView[MAX_PENDING_COUNT];
		std::shared_ptr<Ideal::D3D12UAVBuffer> m_uavBuffer;
		std::shared_ptr<Ideal::D3D12UnorderedAccessView> m_uavView;

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