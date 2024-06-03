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
{
}

Truth::Mesh::~Mesh()
{
}


void Truth::Mesh::SetMesh(std::wstring _path)
{
	m_path = _path;
	m_mesh = m_managers.lock()->Graphics()->CreateMesh(_path);
}

void Truth::Mesh::SetRenderable(bool _isRenderable)
{
	if (_isRenderable)
	{
		m_managers.lock()->Graphics()->AddObject(m_mesh);
	}
}

void Truth::Mesh::Update()
{
	m_mesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
}

void Truth::Mesh::Awake()
{
	SetMesh(m_path);
	SetRenderable(true);
}
