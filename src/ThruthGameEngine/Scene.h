#pragma once
#include "Headers.h"

/// <summary>
/// 게임이 실제로 돌아가는 씬
/// </summary>

class Entity;

class Scene
{
private:
	std::string m_name;
	
	std::list<std::shared_ptr<Entity>> m_entities;

public:
	Scene();
	virtual ~Scene();

public:
	void Update(float4 _dt);
	void Render();
	void LateUpdate(float4 _dt);
	void FixedUpdate(float4 _dt);

	template<typename E>
	void AddEntity();

	virtual void FrontUpdate(float4 _dt) abstract;
	virtual void BackUpdate(float4 _dt) abstract;

	virtual void Enter() abstract;
	virtual void Exit() abstract;
};

template<typename E>
void Scene::AddEntity()
{
	std::shared_ptr<E> entity = std::make_shared<E>();
	m_entities.push_back(entity);
}

