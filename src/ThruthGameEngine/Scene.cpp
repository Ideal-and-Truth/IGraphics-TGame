#include "Scene.h"
#include "Entity.h"
#include "Managers.h"
#include "EventManager.h"

/// <summary>
/// ������
/// </summary>
/// <param name="_managers">�Ŵ���</param>
Truth::Scene::Scene(std::shared_ptr<Managers> _managers)
	: m_managers(_managers)
	, m_name("No Name Scene")
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
	m_awakedEntity.push(_p);
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
	m_beginDestroy.push(_index);
}

/// <summary>
/// ��ƼƼ ����
/// </summary>
/// <param name="_p">������ ��ƼƼ</param>
void Truth::Scene::DeleteEntity(std::shared_ptr<Entity> _p)
{
	m_beginDestroy.push(_p->m_index);
}

/// <summary>
/// �� �ʱ�ȭ (Load Scene �ϴ� ���)
/// </summary>
/// <param name="_manager"></param>
void Truth::Scene::Initalize(std::weak_ptr<Managers> _manager)
{
	m_managers = _manager;
	for (auto& e : m_entities)
	{
		e->SetManager(m_managers);
		m_awakedEntity.push(e);
		e->Initailize();
	}
}

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
		auto e = m_beginDestroy.front();
		m_entities[e]->Destroy();
		std::iter_swap(m_entities.begin() + e, m_entities.begin() + (m_entities.size() - 1));
		m_finishDestroy.push(m_entities[e]);
		m_entities.pop_back();
		m_beginDestroy.pop();
	}

	// Create
	while (!m_awakedEntity.empty())
	{
		auto& e = m_awakedEntity.front();
		e->m_index = static_cast<int32>(m_entities.size());
		m_entities.push_back(e);
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
		e->Update();
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
		e->m_index = static_cast<int32>(m_entities.size());
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

