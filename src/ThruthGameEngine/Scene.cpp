#include "Scene.h"
#include "Entity.h"
#include "Managers.h"
#include "EventManager.h"
#include "GraphicsManager.h"
#include "NavMeshGenerater.h"
#include "PhysicsManager.h"
#include "FileUtils.h"

/// <summary>
/// ������
/// </summary>
/// <param name="_managers">�Ŵ���</param>
Truth::Scene::Scene(std::shared_ptr<Managers> _managers)
	: m_managers(_managers)
	, m_name("No Name Scene")
	, m_mapPath(L"E:\\Projects\\IGraphics-TGame\\src\\Resources\\MapData\\SampleScene.map")
{
}

/// <summary>
/// �Ҹ���
/// </summary>
Truth::Scene::~Scene()
{
	ClearEntity();
}

/// <summary>
/// ��ƼƼ �߰� (Scene ���� ���� ����)
/// </summary>
/// <param name="_p">��ƼƼ</param>
void Truth::Scene::AddEntity(std::shared_ptr<Entity> _p)
{
	_p->m_index = static_cast<int32>(m_entities.size());
	m_entities.push_back(_p);
	_p->Initailize();
	m_awakedEntity.push(_p);

	if (_p->m_parent.expired())
	{
		m_rootEntities.push_back(_p);
	}
}

/// <summary>
/// ��ƼƼ ���� (Scene ���� �� �߰��� �����Ǵ� ��ƼƼ)
/// </summary>
/// <param name="_p"></param>
void Truth::Scene::CreateEntity(std::shared_ptr<Entity> _p)
{
	m_awakedEntity.push(_p);
}

/// <summary>
/// ��ƼƼ ����
/// </summary>
/// <param name="_index">������ ��ƼƼ �ε���</param>
void Truth::Scene::DeleteEntity(uint32 _index)
{
	if (_index >= m_entities.size())
	{
		return;
	}
	m_beginDestroy.push(m_entities[_index]);
}

/// <summary>
/// ��ƼƼ ����
/// </summary>
/// <param name="_p">������ ��ƼƼ</param>
void Truth::Scene::DeleteEntity(std::shared_ptr<Entity> _p)
{
	for (auto& child : _p->m_children)
	{
		DeleteEntity(child);
	}
#ifdef _DEBUG
	if (m_managers.lock()->m_isEdit)
	{
		_p->Destroy();
		m_entities.back()->m_index = _p->m_index;
		std::iter_swap(m_entities.begin() + _p->m_index, m_entities.begin() + (m_entities.size() - 1));
		m_entities.pop_back();
	}
	else
	{
		m_beginDestroy.push(_p);
	}
#else
	m_beginDestroy.push(_p);
#endif // _DEBUG
}

/// <summary>
/// �� �ʱ�ȭ (Load Scene �ϴ� ���)
/// </summary>
/// <param name="_manager"></param>
void Truth::Scene::Initalize(std::weak_ptr<Managers> _manager)
{
	m_managers = _manager;
	for (auto& e : m_rootEntities)
	{
		LoadEntity(e);
	}


	CreateMap(L"E:\\Projects\\IGraphics-TGame\\IGraphics-TGame\\src\\Resources\\MapData\\Level_v0.2.0.map");
}

void Truth::Scene::LoadEntity(std::shared_ptr<Entity> _entity)
{
	m_entities.push_back(_entity);
	_entity->SetManager(m_managers);
	m_awakedEntity.push(_entity);
	_entity->Initailize();

	for (auto& child : _entity->m_children)
	{
		child->m_parent = _entity;
		LoadEntity(child);
	}
}

DirectX::SimpleMath::Vector3 Truth::Scene::FindPath(Vector3 _start, Vector3 _end, Vector3 _size) const 
{
	return m_navMesh->FindPath(_start, _end, _size);
}

std::weak_ptr<Truth::Entity> Truth::Scene::FindEntity(std::string _name)
{
	for (auto& e : m_entities)
	{
		if (e->m_name == _name)
		{
			return e;
		}
	}
	return std::weak_ptr<Entity>();
}

#ifdef _DEBUG
void Truth::Scene::EditorUpdate()
{
	m_rootEntities.clear();
	for (auto& e : m_entities)
	{
		if (e->m_parent.expired())
		{
			m_rootEntities.push_back(e);
		}
	}
}
#endif // _DEBUG

/// <summary>
/// �� ������Ʈ
/// </summary>
void Truth::Scene::Update()
{
	// delete
	while (!m_finishDestroy.empty())
	{
		m_finishDestroy.pop();
	}
	while (!m_beginDestroy.empty())
	{
		auto& e = m_beginDestroy.front();
		e->Destroy();
		m_entities.back()->m_index = e->m_index;
		std::iter_swap(m_entities.begin() + e->m_index, m_entities.begin() + (m_entities.size() - 1));
		m_finishDestroy.push(e);
		m_entities.pop_back();
		m_beginDestroy.pop();
	}

	// Create
	while (!m_awakedEntity.empty())
	{
		auto& e = m_awakedEntity.front();
		e->m_index = static_cast<int32>(m_entities.size());
		m_entities.push_back(e);
#ifdef _DEBUG
		m_rootEntities.push_back(e);
#endif // _DEBUG
		m_startedEntity.push(e);
		e->Awake();
		m_awakedEntity.pop();
	}
	while (!m_startedEntity.empty())
	{
		auto& e = m_startedEntity.front();
		e->Start();
		m_startedEntity.pop();
	}
	for (auto& e : m_entities)
	{
		if (!e->m_isDead)
		{
			e->Update();
		}
		else
		{
			m_beginDestroy.push(e);
		}
	}
}

void Truth::Scene::FixedUpdate()
{
	for (auto& e : m_entities)
	{
		if (!e->m_isDead)
		{
			e->FixedUpdate();
		}
		else
		{
			m_beginDestroy.push(e);
		}
	}
}

void Truth::Scene::LateUpdate()
{
	for (auto& e : m_entities)
	{
		if (!e->m_isDead)
		{
			e->LateUpdate();
		}
		else
		{
			m_beginDestroy.push(e);
		}
	}
}

/// <summary>
/// ��� ���� Transform ���� ����
/// </summary>
void Truth::Scene::ApplyTransform()
{
	for (auto& e : m_entities)
	{
		e->ApplyTransform();
	}
}

/// <summary>
/// �ε� �� ���� ���� �� �� �� �ѹ� ����Ǵ� �Լ�
/// </summary>
void Truth::Scene::Start()
{
	while (!m_awakedEntity.empty())
	{
		auto& e = m_awakedEntity.front();
		m_startedEntity.push(e);
		e->Awake();
		m_awakedEntity.pop();
	}
	while (!m_startedEntity.empty())
	{
		auto& e = m_startedEntity.front();
		e->Start();
		m_startedEntity.pop();
	}
}

/// <summary>
/// �� ���� ��
/// </summary>
void Truth::Scene::Enter()
{
	int32 index = 0;
	for (auto& e : m_entities)
	{
		e->m_index = index++;
	}
}

/// <summary>
/// �� ���� ��
/// </summary>
void Truth::Scene::Exit()
{
	ClearEntity();
}

/// <summary>
/// ��� ��ƼƼ �ʱ�ȭ
/// </summary>
void Truth::Scene::ClearEntity()
{
	m_entities.clear();
}

void Truth::Scene::CreateMap(const std::wstring& _path)
{
	m_managers.lock()->Physics()->CreateMapCollider(_path);

	m_navMesh = std::make_shared<NavMeshGenerater>();
	m_navMesh->Initalize(_path);

// 	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
// 	std::wstring path = _path + L".mmesh";
// 	file->Open(path, FileMode::Read);
// 
// 	uint32 meshCount = file->Read<uint32>();
// 	m_mapMesh.resize(meshCount);
// 
// 	for (uint32 i = 0; i < meshCount; i++)
// 	{
// 		std::string meshpath = file->Read<std::string>();
// 		Matrix meshTM = file->Read<Matrix>();
// 
// 		USES_CONVERSION;
// 		std::wstring wsval(A2W(meshpath.c_str()));
// 
// 		m_mapMesh.push_back(m_managers.lock()->Graphics()->CreateMesh(wsval));
// 		m_mapMesh.back()->SetTransformMatrix(meshTM);
// 	}

	m_mapMesh.push_back(m_managers.lock()->Graphics()->CreateMesh(L"TestMap/Map2"));
}

