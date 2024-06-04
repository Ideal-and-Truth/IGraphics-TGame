#include "SceneManager.h"
#include "Scene.h"

Truth::SceneManager::SceneManager()
{
	DEBUG_PRINT("Create Scene Manager\n");
}

Truth::SceneManager::~SceneManager()
{
	DEBUG_PRINT("Finalize SceneManager\n");
}

void Truth::SceneManager::Initalize(std::shared_ptr<EventManager> _eventManger)
{
	m_eventManager = _eventManger;
	m_eventManager.lock()->Subscribe("Change Scene", MakeListenerInfo(&SceneManager::ChangeScene));
}

void Truth::SceneManager::SetCurrnetScene(std::string _name)
{
	if (!HasScene(_name))
	{
		DEBUG_PRINT("There is no scene with the same name : %s", _name);
		return;
	}
	m_currentScene = m_sceneMap[_name];
}

void Truth::SceneManager::Finalize()
{
	m_currentScene.lock()->Exit();
	m_sceneMap.clear();
}

void Truth::SceneManager::Update() const
{
	m_currentScene.lock()->Update();
}

void Truth::SceneManager::ApplyTransform() const
{
	m_currentScene.lock()->ApplyTransform();
}

void Truth::SceneManager::StartGameScene() const
{
	m_currentScene.lock()->Enter();
}

void Truth::SceneManager::ChangeScene(std::any _p)
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

void Truth::SceneManager::ResetScene(std::any _p) const
{
	m_currentScene.lock()->Exit();
	m_currentScene.lock()->Enter();
}

bool Truth::SceneManager::HasScene(std::string _name)
{
	return m_sceneMap.find(_name) != m_sceneMap.end();
}
