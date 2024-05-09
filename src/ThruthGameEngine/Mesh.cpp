#include "Mesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"

Truth::Mesh::Mesh(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner)
	: Component(_managers, _owner)
{
}

Truth::Mesh::Mesh(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner, std::wstring _path)
	: Component(_managers, _owner)
{
	SetMesh(_path);
	SetRenderable(true);

	EventBind("Update", &Truth::Mesh::Update);
}

Truth::Mesh::~Mesh()
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

void Truth::Mesh::Update(std::any _p)
{
	static bool test = true;
	if (GetKeyDown(KEY::C))
	{
		m_mesh->SetDrawObject(test);
		test = !test;
	}
}
