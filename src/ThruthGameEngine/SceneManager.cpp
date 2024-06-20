#include "SceneManager.h"
#include "Scene.h"

/// <summary>
/// ������
/// </summary>
Truth::SceneManager::SceneManager()
{
	DEBUG_PRINT("Create Scene Manager\n");
}

/// <summary>
/// �Ҹ���
/// </summary>
Truth::SceneManager::~SceneManager()
{
	DEBUG_PRINT("Finalize SceneManager\n");
}

/// <summary>
/// �ʱ�ȭ
/// </summary>
/// <param name="_mangers"></param>
void Truth::SceneManager::Initalize(std::shared_ptr<Managers> _mangers)
{
	m_mangers = _mangers;
	m_eventManager = _mangers->Event();
}

/// <summary>
/// �� �߰�
/// ��� ���� �Ŵ����� �߰� �Ǿ����� ����� �����ϴ�.
/// </summary>
/// <param name="_scene">Scene</param>
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

/// <summary>
/// ���� scene ����
/// </summary>
/// <param name="_name">������Ʈ �� Scene ����</param>
void Truth::SceneManager::SetCurrnetScene(std::string _name)
{
	if (!HasScene(_name))
	{
		DEBUG_PRINT("There is no scene with the same name : %s", _name);
		return;
	}
	if (m_currentScene.expired())
	{
		m_currentScene = m_sceneMap[_name];
	}
	else
	{
		m_currentScene.lock()->Exit();
		m_currentScene = m_sceneMap[_name];
		m_currentScene.lock()->Enter();
	}
}

/// <summary>
/// ����� �Ҹ���
/// </summary>
void Truth::SceneManager::Finalize()
{
	m_currentScene.lock()->Exit();
	m_sceneMap.clear();
}

/// <summary>
/// ������Ʈ
/// </summary>
void Truth::SceneManager::Update() const
{
	m_currentScene.lock()->Update();
}

/// <summary>
/// Transform �Ͻ� ����
/// </summary>
void Truth::SceneManager::ApplyTransform() const
{
	m_currentScene.lock()->ApplyTransform();
}

/// <summary>
/// Scene ����
/// </summary>
void Truth::SceneManager::StartGameScene() const
{
	m_currentScene.lock()->Enter();
}

/// <summary>
/// Scene ����
/// </summary>
/// <param name="_p">������ Scene</param>
void Truth::SceneManager::ChangeScene(std::string&& _name)
{
	m_eventManager.lock()->RemoveAllEvents();
	m_currentScene.lock()->Exit();
	m_currentScene = m_sceneMap[_name];
	m_currentScene.lock()->Enter();
}

/// <summary>
/// Scene ����
/// </summary>
/// <param name="_p"></param>
void Truth::SceneManager::ResetScene() const
{
	m_currentScene.lock()->Exit();
	m_currentScene.lock()->Enter();
}

/// <summary>
/// �ش� ���� �ִ��� Ȯ��
/// </summary>
/// <param name="_name">scene name</param>
/// <returns>���� ����</returns>
bool Truth::SceneManager::HasScene(std::string _name)
{
	return m_sceneMap.find(_name) != m_sceneMap.end();
}

/// <summary>
/// ���� Scene ����
/// </summary>
void Truth::SceneManager::SaveCurrentScene() const
{
	std::ofstream outputstream(m_savedFilePath + m_currentScene.lock()->m_name + ".scene");
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << m_currentScene.lock().get();
}

/// <summary>
/// Scene ����
/// </summary>
void Truth::SceneManager::SaveScene(std::shared_ptr<Scene> _scene) const
{
	std::ofstream outputstream(m_savedFilePath + _scene->m_name + ".scene");
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << _scene;
}

/// <summary>
/// Scene �ε�
/// ���� Scene���� ���� ������ �ʴ´�.
/// </summary>
/// <param name="_path">Scene ���� ���</param>
void Truth::SceneManager::LoadSceneData(std::string _path)
{
	std::ifstream inputstream(_path);
	boost::archive::text_iarchive inputArchive(inputstream);
	std::shared_ptr<Truth::Scene> s;
	inputArchive >> s;

	s->Initalize(m_mangers);

	AddScene(s);
}

void Truth::SceneManager::ReloadSceneData()
{
	std::string sceneName = m_currentScene.lock()->m_name;
	std::ifstream inputstream(m_savedFilePath + sceneName + ".scene");
	boost::archive::text_iarchive inputArchive(inputstream);
	Truth::Scene* s(nullptr);
	inputArchive >> s;

	s->Initalize(m_mangers);

	std::shared_ptr<Truth::Scene> ss = std::shared_ptr<Truth::Scene>(s);

	m_sceneMap[sceneName].reset();
	m_sceneMap[sceneName] = ss;
	m_currentScene = ss;
}
