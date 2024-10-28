#pragma once
#include "Headers.h"

/// <summary>
/// time managing class
/// </summary>
namespace Truth
{
	class EventManager;
	class Managers;

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
		float4 m_fixedTime = 1.0f / 60.0f;
		// 절대 프레임 시간
		float4 m_absDeltaTime;

		float4 m_waitUntilTime;
		float4 m_waitingTime;

		LARGE_INTEGER m_currentCount;
		LARGE_INTEGER m_prevCount;
		LARGE_INTEGER m_frequency;

		std::weak_ptr<EventManager> m_eventManager;
		std::weak_ptr<Managers> m_managers;

	public:
		TimeManager();
		~TimeManager();

		void Initalize(std::shared_ptr<Managers> _managers);
		void Update();
		void Finalize();

		float4 GetDT() const { return m_deltaTime; }
		float4 GetFDT() const { return m_fixedTime; }
		float4 GetADT() const { return m_absDeltaTime; }
		void WaitForSecondsRealtime(float time);
	};

}
