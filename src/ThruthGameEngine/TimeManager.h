#pragma once
#include "Headers.h"

/// <summary>
/// time managing class
/// </summary>
namespace Truth
{
	class EventManager;

	class TimeManager
	{
	private:
		// 프로그램 구동 시간
		float4 m_time;
		// 시간 배율
		float4 m_timeScale;
		// 프레임 시간
		float4 m_deltaTime;
		// 고정 프레임 시간
		float4 m_fixedDeltaTime;
		// 고정 프레임 단위 시간
		float4 m_fixedTime = 0.02f;

		LARGE_INTEGER m_currentCount;
		LARGE_INTEGER m_prevCount;
		LARGE_INTEGER m_frequency;

		std::weak_ptr<EventManager> m_eventManager;

	public:
		TimeManager();
		~TimeManager();

		void Initalize(std::shared_ptr<EventManager> _eventManager);
		void Update();
		void Finalize();

		float4 GetDT() const { return m_deltaTime; }
		float4 GetFDT() const { return m_fixedDeltaTime; }
	};

}
