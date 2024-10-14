#include "SkinnedMesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "IAnimation.h"
#include "Entity.h"
#include "IBone.h"
#include "Imesh.h"
#include "IMaterial.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SkinnedMesh)

Truth::SkinnedMesh::SkinnedMesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
	, m_skinnedMesh(nullptr)
	, m_currentFrame(0)
	, m_isAnimationPlaying(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
	, m_animationMaxFrame(0)
	, m_oldFrame(0)
{
	m_name = "Skinned Mesh Filter";
}


Truth::SkinnedMesh::SkinnedMesh()
	: Component()
	, m_path(L"Kachujin/Mesh")
	, m_isRendering(true)
	, m_skinnedMesh(nullptr)
	, m_currentFrame(0)
	, m_animationMaxFrame(0)
	, m_isAnimationPlaying(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
	, m_oldFrame(0)
{
	m_name = "Skinned Mesh Filter";
}

Truth::SkinnedMesh::~SkinnedMesh()
{
	DeleteMesh();
}

void Truth::SkinnedMesh::SetSkinnedMesh(std::wstring _path)
{
	m_path = _path;
	fs::path meshPath = m_path;

	if (m_skinnedMesh != nullptr)
	{
		m_skinnedMesh.reset();
	}

	m_skinnedMesh = m_managers.lock()->Graphics()->CreateSkinnedMesh(_path);

	m_boneMap.clear();
	uint32 boneSize = m_skinnedMesh->GetBonesSize();

	for (uint32 i = 0; i < boneSize; i++)
	{
		std::weak_ptr<Ideal::IBone> bone = m_skinnedMesh->GetBoneByIndex(i);
		m_boneMap[bone.lock()->GetName()] = bone;
	}

	if (m_matPath.empty())
	{
		for (size_t i = 0; i < m_skinnedMesh->GetMeshesSize(); i++)
		{
			std::string matName = m_skinnedMesh->GetMeshByIndex(static_cast<uint32>(i)).lock()->GetFBXMaterialName();
			fs::path matPath = "../Resources/Matarial" / meshPath.filename();
		
			if (!fs::exists(matPath))
				fs::create_directories(matPath);
		
			matPath = matPath.filename() / matName;
		
			auto material = m_managers.lock()->Graphics()->CreateMaterial(matPath.generic_string());
			m_mat.push_back(material);
		
			m_skinnedMesh->GetMeshByIndex(static_cast<uint32>(i)).lock()->SetMaterialObject(material->m_material);
		}
	}
	else
	{
		for (size_t i = 0; i < m_skinnedMesh->GetMeshesSize(); i++)
		{
			auto material = m_managers.lock()->Graphics()->CreateMaterial(m_matPath[i], false);
			m_mat.push_back(material);

			m_skinnedMesh->GetMeshByIndex(static_cast<uint32>(i)).lock()->SetMaterialObject(material->m_material);
		}
	}
}

void Truth::SkinnedMesh::AddAnimation(std::string _name, std::wstring _path, const Matrix& _offset /*= Matrix::Identity*/)
{
	if (m_animation != nullptr)
	{
		m_animation.reset();
	}

	m_animation = m_managers.lock()->Graphics()->CreateAnimation(_path, _offset);

	if (m_skinnedMesh != nullptr)
	{
		m_skinnedMesh->AddAnimation(_name, m_animation);
	}
}

void Truth::SkinnedMesh::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	if (m_animation != nullptr)
	{
		m_isAnimationChanged = true;
		m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
	}
}

void Truth::SkinnedMesh::SetAnimationSpeed(float Speed)
{
	m_skinnedMesh->SetAnimationSpeed(Speed);
}

void Truth::SkinnedMesh::SetPlayStop(bool playStop)
{
	m_skinnedMesh->SetPlayAnimation(playStop);
}

void Truth::SkinnedMesh::Initialize()
{
	SetSkinnedMesh(m_path);
}

void Truth::SkinnedMesh::FixedUpdate()
{

}

void Truth::SkinnedMesh::Update()
{

	if (!m_isAnimationChanged)
	{
		m_currentFrame = m_skinnedMesh->GetCurrentAnimationIndex();
	}

	if (m_isAnimationChanged)
	{
		if (m_skinnedMesh->GetCurrentAnimationIndex() == 0)
		{
			m_currentFrame = 0;
			m_oldFrame = 0;
			m_isAnimationChanged = false;
		}
	}

	if (m_animation != nullptr)
	{
		m_animationMaxFrame = m_skinnedMesh->GetCurrentAnimationMaxFrame();
		m_skinnedMesh->AnimationDeltaTime(GetDeltaTime());



		if (m_oldFrame > m_currentFrame)
		{
			m_isAnimationEnd = true;
			m_isAnimationPlaying = false;
		}
		else
		{
			m_isAnimationEnd = false;
			m_isAnimationPlaying = true;
		}

		m_oldFrame = m_currentFrame;


	}

}

void Truth::SkinnedMesh::ApplyTransform()
{
	m_skinnedMesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
	m_skinnedMesh->SetDrawObject(m_isRendering);
}

void Truth::SkinnedMesh::DeleteMesh()
{
	m_managers.lock()->Graphics()->DeleteMeshObject(m_skinnedMesh);
}

std::weak_ptr<Ideal::IBone> Truth::SkinnedMesh::GetBone(const std::string& _name)
{
	if (m_boneMap.find(_name) == m_boneMap.end())
	{
		return std::weak_ptr<Ideal::IBone>();
	}
	return m_boneMap[_name];
}

#ifdef EDITOR_MODE
void Truth::SkinnedMesh::EditorSetValue()
{
	m_matPath.clear();
	SetSkinnedMesh(m_path);
}
#endif // EDITOR_MODE

