#pragma once
#include "Headers.h"
#include "EventHandler.h"

/// <summary>
/// 게임이 실제로 돌아가는 씬
/// </summary>
namespace Truth
{
	class Entity;
	class Managers;

	class Scene
		: public EventHandler
	{
	protected:
		std::string m_name;

		std::vector<std::shared_ptr<Entity>> m_entities;

		std::shared_ptr<Managers> m_managers;

	private:
		std::queue<std::shared_ptr<Entity>> m_deletedEntity;
		std::queue<std::shared_ptr<Entity>> m_createdEntity;
		std::queue<std::shared_ptr<Entity>> m_awakeEntity;

	public:
		Scene(std::shared_ptr<Managers> _managers);
		virtual ~Scene();

		// 후에 씬에 오브젝트가 추가되어 다음 프레임부터 Update 시작
		void CreateEntity(std::shared_ptr<Entity> _p);
		void DeleteEntity(std::any _p);

		void Update();

		virtual void Awake() abstract;
		virtual void Enter() abstract;
		virtual void Exit() abstract;

		void ClearEntity();

		void SetManger(std::shared_ptr<Managers> _managers);
	};
}
