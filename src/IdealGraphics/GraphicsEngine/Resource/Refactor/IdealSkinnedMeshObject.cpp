#include "IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/Refactor/IdealAnimation.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/Resource/Refactor/IdealBone.h"

Ideal::IdealSkinnedMeshObject::IdealSkinnedMeshObject()
{

}

Ideal::IdealSkinnedMeshObject::~IdealSkinnedMeshObject()
{

}

void Ideal::IdealSkinnedMeshObject::Init(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ID3D12Device* device = d3d12Renderer->GetDevice().Get();
	{
		const uint32 bufferSize = sizeof(CB_Bone);
		m_cbBone.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
		m_cbBone.GetResource()->SetName(L"Constant Bone");
	}
}

void Ideal::IdealSkinnedMeshObject::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	// TEST
	static float animFrame = 0;
	animFrame += 0.02;
	if (animFrame - 1 > m_animations[0]->frameCount)
	{
		animFrame = 0;
	}
	// Bone Setting
	{
		for (uint32 boneIdx = 0; boneIdx < m_bones.size(); ++boneIdx)
		{
			m_cbBoneData.transforms[boneIdx] = m_animTransforms[0]->transforms[(uint32)animFrame][boneIdx];
		}
	}
	CB_Bone* b = (CB_Bone*)m_cbBone.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	*b = m_cbBoneData;
	
	commandList->SetGraphicsRootConstantBufferView(DYNAMIC_MESH_ROOT_CONSTANT_INDEX + 1, m_cbBone.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));

	
	m_dynamicMesh->Render(Renderer);
}

void Ideal::IdealSkinnedMeshObject::AddAnimation(std::shared_ptr<Ideal::IAnimation> Animation)
{
	m_animations.push_back(std::static_pointer_cast<Ideal::IdealAnimation>(Animation));
	CreateAnimationTransform();
}

void Ideal::IdealSkinnedMeshObject::CreateAnimationTransform()
{
	std::vector<Matrix> tempAnimBoneTransforms(MAX_BONE_TRANSFORMS, Matrix::Identity);

	// Temp : 0번 애니메이션
	std::shared_ptr<Ideal::IdealAnimation> animation = m_animations[0];

	std::shared_ptr<AnimTransform> animTransform = std::make_shared<AnimTransform>();

	for (uint32 frame = 0; frame < animation->frameCount; ++frame)
	{
		for (uint32 boneIdx = 0; boneIdx < m_bones.size(); ++boneIdx)
		{
			std::shared_ptr<Ideal::IdealBone> bone = m_bones[boneIdx];

			Matrix matAnimation;

			std::shared_ptr<ModelKeyframe> keyFrame = animation->GetKeyframe(bone->GetName());

			if (keyFrame != nullptr)
			{
				ModelKeyFrameData& data = keyFrame->transforms[frame];

				Matrix S, R, T;
				S = Matrix::CreateScale(data.scale.x, data.scale.y, data.scale.z);
				R = Matrix::CreateFromQuaternion(data.rotation);
				T = Matrix::CreateTranslation(data.translation.x, data.translation.y, data.translation.z);

				matAnimation = S * R * T;
			}
			else
			{
				matAnimation = Matrix::Identity;
			}

			// 재조립
			Matrix toRootMatrix = bone->GetTransform();
			Matrix invGlobal = toRootMatrix.Invert();

			int32 parentIndex = bone->GetParent();

			Matrix matParent = Matrix::Identity;
			if (parentIndex >= 0)
			{
				matParent = tempAnimBoneTransforms[parentIndex];
			}

			tempAnimBoneTransforms[boneIdx] = matAnimation * matParent;

			// Temp : 0번 애니메이션
			//m_animTransforms[0].transforms[frame][boneIdx] = invGlobal * tempAnimBoneTransforms[boneIdx];
			animTransform->transforms[frame][boneIdx] = invGlobal * tempAnimBoneTransforms[boneIdx];
		}
	}
	m_animTransforms.push_back(animTransform);
}