#pragma once
#include "Headers.h"

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

namespace Truth
{
	class SoundManager
	{
	private:
		static const int MAX_CHANNEL = 512;
		static const int MAX_SOUND = 512;

		FMOD::System* m_system;
		FMOD::Channel* m_channel[MAX_CHANNEL];

		std::unordered_map<fs::path, FMOD::Sound*> m_soundMap;

	public:
		SoundManager();
		~SoundManager();

		void Initalize();
		void Update();
		void Finalize();

		void CreateSound(fs::path _path, bool _isLoop);
		void SetVolum(int _channel, float _vol);
		void Play(fs::path _path, bool _canReduplication, int _channel);
	};
}

