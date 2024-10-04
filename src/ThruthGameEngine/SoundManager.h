#pragma once
#include "Headers.h"

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

enum class SOUND_CHANNEL
{
	BACKGORUND,
	EFFECT,
	ENVIROMENT,
	VIOCE,
	END,
};

namespace Truth
{
	class SoundManager
	{
	private:
		const int MAX_CHANNEL = 512;
		const int MAX_SOUND = 512;

		FMOD::System* m_system;
		FMOD::Sound* m_sound;
		FMOD::Channel* m_channel;

		std::unordered_map<fs::path, FMOD::Sound*> m_soundMap;

	public:
		SoundManager();
		~SoundManager();

		void Initalize();
		void Update();
		void Finalize();

		void CreateSound(fs::path _path, const std::string& _alias);
	};
}

