#include "SceneManager.h"
#include "Scene.h"

SceneManager::SceneManager()
{
	DEBUG_PRINT("Create Scene Manager\n");
}

SceneManager::~SceneManager()
{
	DEBUG_PRINT("Finalize SceneManager\n");
}

void SceneManager::Initalize(std::shared_ptr<EventManager> _eventManger)
{
	m_eventManager = _eventManger;
	m_eventManager.lock()->Subscribe("Change Scene", MakeListenerInfo(&SceneManager::ChangeScene));
}

void SceneManager::SetCurrnetScene(std::string _name)
{
	if (!HasScene(_name))
	{
		DEBUG_PRINT("There is no scene with the same name : %s", _name);
		return;
	}
	m_currentScene = m_sceneMap[_name];
}

void SceneManager::Finalize()
{
	m_currentScene.lock()->Exit();
	m_sceneMap.clear();
}

void SceneManager::StartGameScene()
{
	m_currentScene.lock()->Enter();
}

void SceneManager::ChangeScene(std::any _p)
{
#ifdef _DEBUG
	if (!_p.has_value())
	{
		DEBUG_PRINT("Cannot change Scene : no parameter");
		return;
	}

	if (m_eventManager.expired())
	{
		DEBUG_PRINT("EventManager expired at %s : %d", __FILE__, __LINE__);
		return;
	}
#endif
	m_eventManager.lock()->RemoveAllEvents();
	std::string name = std::any_cast<std::string>(_p);
	m_currentScene.lock()->Exit();
	m_currentScene = m_sceneMap[name];
	m_currentScene.lock()->Enter();
}

void SceneManager::ResetScene(std::any _p)
{
	m_currentScene.lock()->Exit();
	m_currentScene.lock()->Enter();
}

bool SceneManager::HasScene(std::string _name)
{
	return m_sceneMap.find(_name) != m_sceneMap.end();
}
