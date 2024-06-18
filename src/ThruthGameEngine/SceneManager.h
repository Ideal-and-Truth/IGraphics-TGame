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
		std::weak_ptr<Managers> m_mangers;
		const std::string m_savedFilePath = "../Scene/";

	public:
		std::weak_ptr<Scene> m_currentScene;

		SceneManager();
		~SceneManager();

		void Initalize(std::shared_ptr<Managers> _mangers);

		void AddScene(std::shared_ptr<Scene> _scene);

		void Update() const;

		void ApplyTransform() const;

		void StartGameScene() const;

		void ChangeScene(std::string&& _name);
		void ResetScene() const;
		void SetCurrnetScene(std::string _name);

		void Finalize();

		void SaveSceneData() const;
		void LoadSceneData(std::string _path);
		void ReloadSceneData();

	private:
		bool HasScene(std::string _name);
	};

}

