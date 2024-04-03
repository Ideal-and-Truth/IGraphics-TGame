#pragma once
#include "Headers.h"

class TimeManager;
class InputManager;

class Processor
{
private:
	std::unique_ptr<TimeManager> m_timeManager;
	std::unique_ptr<InputManager> m_inputManager;

public:
	Processor();
	~Processor();

	void Initialize();
	void Process();

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();
};

