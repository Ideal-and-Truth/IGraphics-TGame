#include "Mesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "Entity.h"

Truth::Mesh::Mesh()
	: Component()
{
}

Truth::Mesh::Mesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
{
	m_name = "Mesh Filter";
}

Truth::Mesh::~Mesh()
{
}


void Truth::Mesh::SetMesh(std::wstring _path)
{
	m_path = _path;
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

void Truth::Mesh::Update()
{
	m_mesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
	m_mesh->SetDrawObject(m_isRendering);
}

void Truth::Mesh::Awake()
{
	m_name = "Mesh Filter";
	SetMesh(m_path);
	SetRenderable(true);
}
