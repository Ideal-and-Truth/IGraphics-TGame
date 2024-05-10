#include "Scene.h"
#include "Entity.h"

Truth::Scene::Scene(std::shared_ptr<Managers> _managers)
	: m_managers(_managers)
{

}

Truth::Scene::~Scene()
{
	ClearEntity();
}

void Truth::Scene::AddEntity(std::shared_ptr<Entity> _entity)
{
	_entity->SetManager(m_managers);
	_entity->Initailize();
	m_entities.push_back(_entity);
}

void Truth::Scene::ClearEntity()
{
	m_entities.clear();
}

void Truth::Scene::SetManger(std::shared_ptr<Managers> _managers)
{
	m_managers = _managers;
}

