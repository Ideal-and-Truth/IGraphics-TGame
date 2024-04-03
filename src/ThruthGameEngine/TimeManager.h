#pragma once
#include "Headers.h"

/// <summary>
/// time managing class
/// </summary>
class TimeManager
{
private:
	float4 m_deltaTime;

	LARGE_INTEGER m_currentCount;
	LARGE_INTEGER m_prevCount;
	LARGE_INTEGER m_frequency;

public:
	TimeManager();
	~TimeManager();

	void Initalize();
	void Update();

	float4 GetDT() const { return m_deltaTime; }
};

