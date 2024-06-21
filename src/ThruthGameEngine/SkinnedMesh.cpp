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
	, m_isAnimationEnd(false)
{
	m_name = "Skinned Mesh Filter";
}


Truth::SkinnedMesh::SkinnedMesh()
	: Component()
	, m_path(L"Kachujin/Mesh")
	, m_isRendering(true)
	, m_skinnedMesh(nullptr)
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

	if (m_isRendering)
	{
		m_managers.lock()->Graphics()->AddObject(m_skinnedMesh);
	}
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
	}
}

void Truth::SkinnedMesh::SetRenderable(bool _isRenderable)
{
	if (_isRenderable)
	{
		m_managers.lock()->Graphics()->AddObject(m_skinnedMesh);
	}
}

void Truth::SkinnedMesh::Initalize()
{
	SetSkinnedMesh(m_path);
}

void Truth::SkinnedMesh::FixedUpdate()
{
	m_currentFrame = m_skinnedMesh->GetCurrentAnimationIndex();
	if (m_currentFrame == 0)
	{
		m_isAnimationEnd = true;
	}
	else
	{
		m_isAnimationEnd = false;
	}
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

