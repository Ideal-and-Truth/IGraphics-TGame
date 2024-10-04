#include "SoundManager.h"
#include "fmod.hpp"
#include "fmod_errors.h"

Truth::SoundManager::SoundManager()
	: m_system(nullptr)
	, m_sound(nullptr)
	, m_channel(nullptr)
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_system);
	assert(result != FMOD_OK && "cannnot create fmod system");
}

Truth::SoundManager::~SoundManager()
{

}

void Truth::SoundManager::Initalize()
{
	FMOD_RESULT result;
	result = m_system->init(MAX_CHANNEL, FMOD_INIT_NORMAL, 0);
	assert(result != FMOD_OK && "cannnot initalize fmod system");

}

void Truth::SoundManager::Update()
{
	m_system->update();
}

void Truth::SoundManager::Finalize()
{

}

void Truth::SoundManager::CreateSound(fs::path _path, const std::string& _alias)
{
}
