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

void Truth::SceneManager::Initalize(std::shared_ptr<Managers> _mangers)
{
	m_mangers = _mangers;
	m_eventManager = _mangers->Event();
	m_eventManager.lock()->Subscribe("Change Scene", MakeListenerInfo(&SceneManager::ChangeScene));
}

void Truth::SceneManager::AddScene(std::shared_ptr<Scene> _scene)
{
	std::string& sName = _scene->m_name;
	if (HasScene(sName))
	{
		DEBUG_PRINT("There is a scene with the same name : %s", sName);
		return;
	}
	m_sceneMap[sName] = _scene;
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

void Truth::SceneManager::SaveSceneData() const
{
	std::ofstream outputstream(m_savedFilePath + m_currentScene.lock()->m_name + ".scene");
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << m_currentScene.lock().get();
}

void Truth::SceneManager::LoadSceneData(std::string _path)
{
	std::ifstream inputstream(_path);
	boost::archive::text_iarchive inputArchive(inputstream);
	Truth::Scene* s(nullptr);
	inputArchive >> s;

	s->Initalize(m_mangers);
	
	std::shared_ptr<Truth::Scene> ss = std::shared_ptr<Truth::Scene>(s);

	AddScene(ss);
}
