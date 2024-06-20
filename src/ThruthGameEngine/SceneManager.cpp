#include "SceneManager.h"
#include "Scene.h"

/// <summary>
/// 생성자
/// </summary>
Truth::SceneManager::SceneManager()
{
	DEBUG_PRINT("Create Scene Manager\n");
}

/// <summary>
/// 소멸자
/// </summary>
Truth::SceneManager::~SceneManager()
{
	DEBUG_PRINT("Finalize SceneManager\n");
}

/// <summary>
/// 초기화
/// </summary>
/// <param name="_mangers"></param>
void Truth::SceneManager::Initalize(std::shared_ptr<Managers> _mangers)
{
	m_mangers = _mangers;
	m_eventManager = _mangers->Event();
}

/// <summary>
/// 씬 추가
/// 모든 씬은 매니저에 추가 되어져야 사용이 가능하다.
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
/// 현재 scene 지정
/// </summary>
/// <param name="_name">업데이트 될 Scene 지정</param>
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
/// 명시적 소멸자
/// </summary>
void Truth::SceneManager::Finalize()
{
	m_currentScene.lock()->Exit();
	m_sceneMap.clear();
}

/// <summary>
/// 업데이트
/// </summary>
void Truth::SceneManager::Update() const
{
	m_currentScene.lock()->Update();
}

/// <summary>
/// Transform 일시 적용
/// </summary>
void Truth::SceneManager::ApplyTransform() const
{
	m_currentScene.lock()->ApplyTransform();
}

/// <summary>
/// Scene 시작
/// </summary>
void Truth::SceneManager::StartGameScene() const
{
	m_currentScene.lock()->Enter();
}

/// <summary>
/// Scene 변경
/// </summary>
/// <param name="_p">변경할 Scene</param>
void Truth::SceneManager::ChangeScene(std::string&& _name)
{
	m_eventManager.lock()->RemoveAllEvents();
	m_currentScene.lock()->Exit();
	m_currentScene = m_sceneMap[_name];
	m_currentScene.lock()->Enter();
}

/// <summary>
/// Scene 리셋
/// </summary>
/// <param name="_p"></param>
void Truth::SceneManager::ResetScene() const
{
	m_currentScene.lock()->Exit();
	m_currentScene.lock()->Enter();
}

/// <summary>
/// 해당 씬이 있는지 확인
/// </summary>
/// <param name="_name">scene name</param>
/// <returns>보유 여부</returns>
bool Truth::SceneManager::HasScene(std::string _name)
{
	return m_sceneMap.find(_name) != m_sceneMap.end();
}

/// <summary>
/// 현재 Scene 저장
/// </summary>
void Truth::SceneManager::SaveCurrentScene() const
{
	std::ofstream outputstream(m_savedFilePath + m_currentScene.lock()->m_name + ".scene");
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << m_currentScene.lock().get();
}

/// <summary>
/// Scene 저장
/// </summary>
void Truth::SceneManager::SaveScene(std::shared_ptr<Scene> _scene) const
{
	std::ofstream outputstream(m_savedFilePath + _scene->m_name + ".scene");
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << _scene;
}

/// <summary>
/// Scene 로드
/// 현재 Scene으로 지정 하지는 않는다.
/// </summary>
/// <param name="_path">Scene 파일 경로</param>
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
