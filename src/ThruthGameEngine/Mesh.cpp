#include "Mesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "Entity.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Mesh)

Truth::Mesh::Mesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
	, m_mesh(nullptr)
{
	m_name = "Mesh Filter";
}


Truth::Mesh::Mesh()
	: Component()
	, m_path(L"DebugObject/debugCube")
	, m_isRendering(true)
	, m_mesh(nullptr)
{
	m_name = "Mesh Filter";
}

Truth::Mesh::~Mesh()
{
}

void Truth::Mesh::SetMesh(std::wstring _path)
{
	m_path = _path;

	if (m_mesh != nullptr)
	{
		m_mesh.reset();
	}

	m_mesh = m_managers.lock()->Graphics()->CreateMesh(_path);

	if (m_isRendering)
	{
		m_managers.lock()->Graphics()->AddObject(m_mesh);
	}
}

void Truth::Mesh::SetRenderable(bool _isRenderable)
{
	if (_isRenderable)
	{
		m_managers.lock()->Graphics()->AddObject(m_mesh);
	}
}

void Truth::Mesh::Initalize()
{
	SetMesh(m_path);
}

void Truth::Mesh::ApplyTransform()
{
	m_mesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
	m_mesh->SetDrawObject(m_isRendering);
}

