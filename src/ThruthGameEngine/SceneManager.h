#pragma once
#include "Headers.h"
#include "EventHandler.h"

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
		std::weak_ptr<Scene> m_currentScene;
		std::weak_ptr<EventManager> m_eventManager;

	public:
		SceneManager();
		~SceneManager();

		void Initalize(std::shared_ptr<EventManager> _eventManger);

		template<typename S, typename std::enable_if<std::is_base_of_v<Scene, S>, S>::type* = nullptr>
		void AddScene(std::string _name, std::shared_ptr<Managers> _managers);

		void Update();

		void StartGameScene();

		void ChangeScene(std::any _p);
		void ResetScene(std::any _p);
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
	}
}

