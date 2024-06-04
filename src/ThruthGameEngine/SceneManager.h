#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Scene.h"
namespace Truth
{
	class Scene;
	class EventManager;
	class Managers;
}

namespace Truth
{
	class SceneManager
		: public EventHandler
	{
	private:
		std::map<std::string, std::shared_ptr<Scene>> m_sceneMap;
		std::weak_ptr<EventManager> m_eventManager;

	public:
		std::weak_ptr<Scene> m_currentScene;

		SceneManager();
		~SceneManager();

		void Initalize(std::shared_ptr<EventManager> _eventManger);

		template<typename S, typename std::enable_if<std::is_base_of_v<Scene, S>, S>::type* = nullptr>
		void AddScene(std::string _name, std::shared_ptr<Managers> _managers);

		void Update() const;

		void ApplyTransform() const;

		void StartGameScene() const;

		void ChangeScene(std::any _p);
		void ResetScene(std::any _p) const;
		void SetCurrnetScene(std::string _name);

		void Finalize();

	private:
		bool HasScene(std::string _name);
	};
	template<typename S, typename std::enable_if<std::is_base_of_v<Scene, S>, S>::type*>
	void SceneManager::AddScene(std::string _name, std::shared_ptr<Managers> _managers)
	{
		if (HasScene(_name))
		{
			DEBUG_PRINT("There is a scene with the same name : %s", _name);
			return;
		}
		m_sceneMap[_name] = std::make_shared<S>(_managers);
		m_sceneMap[_name]->m_name = _name;
	}
}

