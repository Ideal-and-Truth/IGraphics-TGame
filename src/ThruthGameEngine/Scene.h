#pragma once
#include "Headers.h"

/// <summary>
/// 게임이 실제로 돌아가는 씬
/// </summary>
namespace Truth
{
	class Entity;
	class Managers;
	class Scene
	{
	protected:
		std::string m_name;

		std::vector<std::shared_ptr<Entity>> m_entities;

		std::weak_ptr<Managers> m_managers;

	public:
		Scene(std::shared_ptr<Managers> _managers);
		virtual ~Scene();

	public:
		template<typename E>
		void AddEntity();

		virtual void Awake() abstract;
		virtual void Enter() abstract;
		virtual void Exit() abstract;

		void ClearEntity();
	};
}

template<typename E>
void Truth::Scene::AddEntity()
{
	std::shared_ptr<E> entity = std::make_shared<E>();
	m_entities.push_back(entity);
	entity->SetManager(m_managers);
	entity->Initailize();
}

