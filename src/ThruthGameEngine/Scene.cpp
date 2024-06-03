#include "Scene.h"
#include "Entity.h"
#include "Managers.h"
#include "EventManager.h"

Truth::Scene::Scene(std::shared_ptr<Managers> _managers)
	: m_managers(_managers)
{
	m_managers->Event()->Subscribe("Delete", MakeListenerInfo(&Truth::Scene::DeleteEntity));
}

Truth::Scene::~Scene()
{
	ClearEntity();
}

void Truth::Scene::CreateEntity(std::shared_ptr<Entity> _p)
{
	m_createdEntity.push(_p);
}

void Truth::Scene::DeleteEntity(std::any _p)
{
	m_deletedEntity.push(std::any_cast<std::shared_ptr<Entity>>(_p));
}

void Truth::Scene::Update()
{
	// delete
	while (!m_deletedEntity.empty())
	{
		auto& e = m_deletedEntity.front();
		e->Destroy();
		m_deletedEntity.pop();
	}

	// Create
	while (!m_createdEntity.empty())
	{
		auto& e = m_createdEntity.front();
		m_entities.push_back(e);
		e->SetManager(m_managers);
		e->Awake();
		m_startedEntity.push(e);
		m_createdEntity.pop();
	}

	// Start
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

void Truth::Scene::ApplyTransform()
{
	for (auto& e : m_entities)
	{
		e->ApplyTransform();
	}
}

void Truth::Scene::ClearEntity()
{
	m_entities.clear();
}

void Truth::Scene::SetManger(std::shared_ptr<Managers> _managers)
{
	m_managers = _managers;
}

