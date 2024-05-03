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
		// ���α׷� ���� �ð�
		float4 m_time;
		// �ð� ����
		float4 m_timeScale;
		// ������ �ð�
		float4 m_deltaTime;
		// ���� ������ �ð�
		float4 m_fixedDeltaTime;
		// ���� ������ ���� �ð�
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
