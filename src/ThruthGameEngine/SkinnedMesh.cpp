#include "SkinnedMesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "Entity.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SkinnedMesh)

Truth::SkinnedMesh::SkinnedMesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
	, m_skinnedMesh(nullptr)
	, m_currentFrame(0)
	, m_isAnimationStart(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
{
	m_name = "Skinned Mesh Filter";
}


Truth::SkinnedMesh::SkinnedMesh()
	: Component()
	, m_path(L"Kachujin/Mesh")
	, m_isRendering(true)
	, m_skinnedMesh(nullptr)
	, m_currentFrame(0)
	, m_isAnimationStart(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
{
	m_name = "Skinned Mesh Filter";
}

Truth::SkinnedMesh::~SkinnedMesh()
{
}

void Truth::SkinnedMesh::SetSkinnedMesh(std::wstring _path)
{
	m_path = _path;

	if (m_skinnedMesh != nullptr)
	{
		m_skinnedMesh.reset();
	}

	m_skinnedMesh = m_managers.lock()->Graphics()->CreateSkinnedMesh(_path);
}

void Truth::SkinnedMesh::AddAnimation(std::string _name, std::wstring _path)
{
	if (m_animation != nullptr)
	{
		m_animation.reset();
	}

	m_animation = m_managers.lock()->Graphics()->CreateAnimation(_path);

	if (m_skinnedMesh != nullptr)
	{
		m_skinnedMesh->AddAnimation(_name, m_animation);
	}
}

void Truth::SkinnedMesh::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	if (m_animation != nullptr)
	{
		m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
		m_isAnimationChanged = true;
	}
}

void Truth::SkinnedMesh::SetRenderable(bool _isRenderable)
{
}

void Truth::SkinnedMesh::Initalize()
{
	SetSkinnedMesh(m_path);
}

void Truth::SkinnedMesh::FixedUpdate()
{
	// 원래는 프레임 단위여야되지만 지금 애니메이션의 마지막 프레임을 몰라서
	// 업데이트에 넣는게 유연한듯
	m_currentFrame = m_skinnedMesh->GetCurrentAnimationIndex();
	if (m_currentFrame == 0)
	{
		m_isAnimationEnd = true;
	}
	else if (m_currentFrame == 1)
	{
		m_isAnimationStart = true;
		m_isAnimationChanged = false;
	}
	else
	{
		m_isAnimationStart = false;
		m_isAnimationEnd = false;
	}
	
}

void Truth::SkinnedMesh::Update()
{

}

void Truth::SkinnedMesh::ApplyTransform()
{
	m_skinnedMesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
	m_skinnedMesh->SetDrawObject(m_isRendering);
}

#ifdef _DEBUG
void Truth::SkinnedMesh::EditorSetValue()
{
	SetSkinnedMesh(m_path);
}
#endif // _DEBUG

