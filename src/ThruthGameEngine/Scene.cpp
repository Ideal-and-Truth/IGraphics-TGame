#include "Scene.h"
#include "Entity.h"

Scene::Scene(std::shared_ptr<Managers> _managers)
	: m_managers(_managers)
{

}

Scene::~Scene()
{
	ClearEntity();
}

void Scene::ClearEntity()
{
	m_entities.clear();
}

