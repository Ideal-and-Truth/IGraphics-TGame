#include "Scene.h"
#include "Entity.h"

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::Update(float4 _dt)
{
	FrontUpdate(_dt);
	for (auto& e : m_entities)
	{
		e->Update(_dt);
	}
	BackUpdate(_dt);
}

void Scene::Render()
{
	for (auto& e : m_entities)
	{
		e->Render();
	}
}

void Scene::LateUpdate(float4 _dt)
{
	for (auto& e : m_entities)
	{
		e->LateUpdate(_dt);
	}
}

void Scene::FixedUpdate(float4 _dt)
{
	for (auto& e : m_entities)
	{
		e->FixedUpdate(_dt);
	}
}
