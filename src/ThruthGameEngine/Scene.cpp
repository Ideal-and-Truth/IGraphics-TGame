#include "Scene.h"
#include "Entity.h"
#include "Managers.h"
#include "EventManager.h"
#include "GraphicsManager.h"

/// <summary>
/// 생성자
/// </summary>
/// <param name="_managers">매니저</param>
Truth::Scene::Scene(std::shared_ptr<Managers> _managers)
	: m_managers(_managers)
	, m_name("No Name Scene")
{
}

/// <summary>
/// 소멸자
/// </summary>
Truth::Scene::~Scene()
{
	ClearEntity();
}

/// <summary>
/// 엔티티 추가 (Scene 시작 전에 들어가는)
/// </summary>
/// <param name="_p">엔티티</param>
void Truth::Scene::AddEntity(std::shared_ptr<Entity> _p)
{
	_p->m_index = static_cast<int32>(m_entities.size());
	m_entities.push_back(_p);
	_p->Initailize();
	m_awakedEntity.push(_p);
#ifdef _DEBUG
	m_rootEntities.push_back(_p);
#endif // _DEBUG
}

/// <summary>
/// 엔티티 생성 (Scene 시작 후 중간에 생성되는 엔티티)
/// </summary>
/// <param name="_p"></param>
void Truth::Scene::CreateEntity(std::shared_ptr<Entity> _p)
{
	m_awakedEntity.push(_p);
}

/// <summary>
/// 엔티티 삭제
/// </summary>
/// <param name="_index">삭제될 엔티티 인덱스</param>
void Truth::Scene::DeleteEntity(uint32 _index)
{
	if (_index >= m_entities.size())
	{
		return;
	}
	m_beginDestroy.push(m_entities[_index]);
}

/// <summary>
/// 엔티티 삭제
/// </summary>
/// <param name="_p">삭제될 엔티티</param>
void Truth::Scene::DeleteEntity(std::shared_ptr<Entity> _p)
{
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
/// 씬 초기화 (Load Scene 하는 경우)
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
#ifdef _DEBUG
		m_rootEntities.push_back(e);
#endif // _DEBUG
	}
}

#ifdef _DEBUG
void Truth::Scene::EditorUpdate()
{
	for (auto i = m_rootEntities.begin(); i != m_rootEntities.end(); )
	{
		if (!(*i)->m_parent.expired())
		{
			i = m_rootEntities.erase(i);
		}
		else
		{
			i++;
		}
	}
}
#endif // _DEBUG

/// <summary>
/// 씬 업데이트
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
/// 모든 씬의 Transform 정보 갱신
/// </summary>
void Truth::Scene::ApplyTransform()
{
	for (auto& e : m_entities)
	{
		e->ApplyTransform();
	}
}

/// <summary>
/// 로드 된 씬을 시작 할 때 단 한번 실행되는 함수
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
/// 씬 진입 시
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
/// 씬 나갈 시
/// </summary>
void Truth::Scene::Exit()
{
	ClearEntity();
	m_managers.lock()->Graphics()->ResetRenderScene();
}

/// <summary>
/// 모든 엔티티 초기화
/// </summary>
void Truth::Scene::ClearEntity()
{
	m_entities.clear();
}

