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
		m_cbBone.GetResource()->SetName(L"CB_Bone");
	}
	{
		const uint32 bufferSize = sizeof(CB_Transform);
		m_cbTransform.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
		m_cbTransform.GetResource()->SetName(L"CB_Transform");
	}
}

void Ideal::IdealSkinnedMeshObject::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	AnimationPlay();

	CB_Transform* t = (CB_Transform*)m_cbTransform.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	//*t = m_cbTransformData;
	t->World = m_transform;
	t->View = d3d12Renderer->GetView();
	t->Proj = d3d12Renderer->GetProj();
	t->WorldInvTranspose = m_transform.Invert();
	commandList->SetGraphicsRootConstantBufferView(DYNAMIC_MESH_ROOT_CONSTANT_INDEX, m_cbTransform.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));

	CB_Bone* b = (CB_Bone*)m_cbBone.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	*b = m_cbBoneData;
	commandList->SetGraphicsRootConstantBufferView(DYNAMIC_MESH_ROOT_CONSTANT_INDEX + 1, m_cbBone.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));



	m_skinnedMesh->Render(Renderer);
}

void Ideal::IdealSkinnedMeshObject::SetAnimation(const std::string& AnimationName, bool WhenCurrentAnimationFinished /*= true*/)
{
	m_whenCurrentAnimationFinishChangeAnimation = WhenCurrentAnimationFinished;
	m_nextAnimation = m_animations2[AnimationName];
	if (m_nextAnimation == nullptr)
	{
		MessageBoxA(NULL, "NullException - SetAnimation", "SetAnimation", MB_OK);
		assert(false);
	}
}

void Ideal::IdealSkinnedMeshObject::AddAnimation(const std::string& AnimationName, std::shared_ptr<Ideal::IAnimation> Animation)
{
	if (m_animations2[AnimationName] != nullptr)
	{
		MessageBoxA(NULL, "Already Have Same Name Animation", "AddAnimation Error", MB_OK);
		return;
	}
	m_animations2[AnimationName] = std::static_pointer_cast<Ideal::IdealAnimation>(Animation);
	if (m_currentAnimation == nullptr)
	{
		m_currentAnimation = std::static_pointer_cast<Ideal::IdealAnimation>(Animation);
	}
}

void Ideal::IdealSkinnedMeshObject::SetSkinnedMesh(std::shared_ptr<Ideal::IdealSkinnedMesh> Mesh)
{
	m_skinnedMesh = Mesh;

	auto bones = Mesh->GetBones();
	for (auto b : bones)
	{
		std::shared_ptr<Ideal::IdealBone> newBone = std::make_shared<Ideal::IdealBone>();
		newBone->SetName(b->GetName());
		newBone->SetBoneIndex(b->GetBoneIndex());
		newBone->SetParent(b->GetParent());
		newBone->SetTransform(b->GetTransform());
		m_bones.push_back(newBone);
	}
}

void Ideal::IdealSkinnedMeshObject::AnimationPlay()
{
	m_sumTime += 0.01f;

	// ���� �ִϸ��̼��� �����µ� ���� �ִϸ��̼��� ���� ���?
	bool changeAnimationFlag = false;
	if (m_isAnimationFinished)
	{
		if (m_nextAnimation)
		{
			if (m_whenCurrentAnimationFinishChangeAnimation)
			{
				float timePerFrame = 1 / (m_currentAnimation->frameRate * m_animSpeed);

				for (uint32 boneIdx = 0; boneIdx < m_bones.size(); ++boneIdx)
				{
					Matrix currentFrame = m_currentAnimation->m_animTransform->transforms[m_currentAnimation->frameCount - 1][boneIdx];
					Matrix nextFrame = m_nextAnimation->m_animTransform->transforms[0][boneIdx];
					Matrix resultFrame = Matrix::Identity;
					Matrix::Lerp(currentFrame, nextFrame, m_ratio, resultFrame);
					m_cbBoneData.transforms[boneIdx] = resultFrame;
				}
				m_ratio = m_sumTime / timePerFrame;

				// ������ �������� �ִϸ��̼��� �����ٴ� ��
				if (m_ratio >= 1.0f)
				{
					m_currentAnimation = m_nextAnimation;
					m_nextAnimation = nullptr;
					m_currentFrame = 0;
					m_nextFrame = 0;
					m_isAnimationFinished = false;
					m_whenCurrentAnimationFinishChangeAnimation = false;
				}

				changeAnimationFlag = true;
			}
			else
			{
				m_currentAnimation = m_nextAnimation;
				m_nextAnimation = nullptr;
				m_currentFrame = 0;
				m_nextFrame = 0;
				m_isAnimationFinished = false;
			}
		}
		else
		{
			// next animation�� ���� ��� �ٽ� ���
			m_isAnimationFinished = false;
		}
	}

	if (changeAnimationFlag == false && m_currentAnimation)
	{
		// Bone Setting
		{
			float timePerFrame = 1 / (m_currentAnimation->frameRate * m_animSpeed);
			if (m_sumTime >= timePerFrame)
			{
				m_sumTime = 0.f;
				// ���� ������ + 1�� ���� �ִϸ��̼��� �ִ� ������ - 1 ���� Ŭ ��� �ִϸ��̼��� �����ٰ� ó���Ѵ�.
				if (m_currentFrame + 1 > m_currentAnimation->frameCount - 1)
				{
					m_isAnimationFinished = true;
				}
				m_currentFrame = (m_currentFrame + 1) % m_currentAnimation->frameCount;
				m_nextFrame = (m_currentFrame + 1) % m_currentAnimation->frameCount;
			}

			m_ratio = m_sumTime / timePerFrame;

			for (uint32 boneIdx = 0; boneIdx < m_bones.size(); ++boneIdx)
			{
				// Current Frame
				Matrix currentFrame = m_currentAnimation->m_animTransform->transforms[m_currentFrame][boneIdx];
				Matrix nextFrame = m_currentAnimation->m_animTransform->transforms[m_nextFrame][boneIdx];
				Matrix resultFrame = Matrix::Identity;
				Matrix::Lerp(currentFrame, nextFrame, m_ratio, resultFrame);
				m_cbBoneData.transforms[boneIdx] = resultFrame;
			}
		}
	}

}
