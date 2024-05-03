#include "Mesh.h"
#include "Managers.h"
#include "GraphicsManager.h"

Truth::Mesh::Mesh()
{
}

Truth::Mesh::~Mesh()
{
}

void Truth::Mesh::Awake()
{
}

void Truth::Mesh::SetMesh(std::wstring _path)
{
	m_path = _path;
	m_mesh = m_managers.lock()->Graphics()->CreateSkinnedMesh(_path);
}

void Truth::Mesh::SetRenderable(bool _isRenderable)
{
	if (_isRenderable)
	{
		m_managers.lock()->Graphics()->AddObject(m_mesh);
	}
}
