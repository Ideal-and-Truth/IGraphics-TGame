#include "SoundManager.h"
#include "fmod.hpp"
#include "fmod_errors.h"

Truth::SoundManager::SoundManager()
	: m_system(nullptr)
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_system);
	assert(result == FMOD_OK && "cannnot create fmod system");
}

Truth::SoundManager::~SoundManager()
{

}

void Truth::SoundManager::Initalize()
{
	FMOD_RESULT result;
	result = m_system->init(MAX_CHANNEL, FMOD_INIT_NORMAL, 0);
	assert(result == FMOD_OK && "cannnot initalize fmod system");

}

void Truth::SoundManager::Update()
{
	m_system->update();
}

void Truth::SoundManager::Finalize()
{
	for (auto& s : m_soundMap)
	{
		s.second->release();
	}
	m_system->release();
}

void Truth::SoundManager::CreateSound(fs::path _path, bool _isLoop)
{
// 	std::unordered_map<fs::path, FMOD::Sound*>::iterator itr = m_soundMap.find(_path);
// 	if (itr != m_soundMap.end())
// 	{
// 		return;
// 	}
// 
// 	FMOD_RESULT result;
// 	if (_isLoop == true)
// 		result = m_system->createSound(_path.generic_string().c_str(), FMOD_LOOP_NORMAL, 0, &m_soundMap[_path]);
// 	else
// 		result = m_system->createSound(_path.generic_string().c_str(), FMOD_LOOP_OFF, 0, &m_soundMap[_path]);
}

void Truth::SoundManager::SetVolum(int _channel, float _vol)
{
// 	FMOD_RESULT result;
// 
// 	result = m_channel[_channel]->setVolume(_vol);
}

void Truth::SoundManager::Play(fs::path _path, bool _canReduplication, int _channel)
{
// 	std::unordered_map<fs::path, FMOD::Sound*>::iterator itr = m_soundMap.find(_path);
// 	if (itr == m_soundMap.end())
// 	{
// 		return;
// 	}
// 	FMOD_RESULT result;
// 
// 	bool isPlaying = false;
// 	m_channel[_channel]->isPlaying(&isPlaying);
// 
// 	if (_canReduplication)
// 		result = m_system->playSound((*itr).second, nullptr, false, &m_channel[_channel]);
// 	else if (!_canReduplication && !isPlaying)
// 		result = m_system->playSound((*itr).second, nullptr, false, &m_channel[_channel]);
}
