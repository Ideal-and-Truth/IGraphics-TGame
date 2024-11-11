#include "StageBGM.h"
#include "SoundManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(StageBGM)

StageBGM::StageBGM()
	: m_isPlay(false)
	, m_sceneName("")
{
	m_name = "StageBGM";
}

StageBGM::~StageBGM()
{
	m_managers.lock()->Sound()->Stop(64);
	m_managers.lock()->Sound()->Stop(65);
}

void StageBGM::Start()
{
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\00. Title_Sound\\Title_Sound_Fix.wav", true);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_1_BGM.wav", true);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_2_BGM.wav", true);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_3_BGM.wav", true);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_1_Ambient_Sound.wav", true);
}

void StageBGM::Update()
{
	std::string sceneName = m_managers.lock()->Scene()->m_currentScene->m_name;

	if (m_sceneName != sceneName)
	{
		m_isPlay = false;
		m_managers.lock()->Sound()->Stop(64);
		m_managers.lock()->Sound()->Stop(65);
	}

	if (!m_isPlay)
	{
		if (sceneName == "TitleScene")
		{
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\00. Title_Sound\\Title_Sound_Fix.wav", false, 64);
			m_isPlay = true;
			m_sceneName = sceneName;
		}

		if (sceneName == "Stage1"|| sceneName == "test")
		{
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_1_BGM.wav", false, 64);
			m_managers.lock()->Sound()->SetVolum(64, 0.5f);
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_1_Ambient_Sound.wav", false, 65);
			m_isPlay = true;
			m_sceneName = sceneName;
		}

		if (sceneName == "Stage2")
		{
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_2_BGM.wav", false, 64);
			m_managers.lock()->Sound()->SetVolum(64, 1.f);
			m_isPlay = true;
			m_sceneName = sceneName;
		}

		if (sceneName == "Stage3")
		{
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_3_BGM.wav", false, 64);
			m_managers.lock()->Sound()->SetVolum(64, 1.f);
			m_isPlay = true;
			m_sceneName = sceneName;
		}

		if (sceneName == "StageBoss")
		{
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_1_BGM.wav", false, 64);
			m_managers.lock()->Sound()->SetVolum(64, 0.5f);
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\01. Ambient_Sound\\Stage_1_Ambient_Sound.wav", false, 65);
			m_isPlay = true;
			m_sceneName = sceneName;
		}
	}

}
