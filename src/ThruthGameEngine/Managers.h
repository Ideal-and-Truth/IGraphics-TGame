#pragma once
#include "Headers.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Eventmanager.h"

class Managers
	: public Singleton<Managers>
{
private:
	std::shared_ptr<TimeManager> m_timeManager;
	std::shared_ptr<InputManager> m_inputManager;
	std::shared_ptr<EventManager> m_eventManager;

private:
	friend class Singleton<Managers>;
	Managers() = default;
	~Managers() = default;

public:

	void Initialize(HWND _hwnd);
	void Update();

	std::shared_ptr<TimeManager> Time() { return m_timeManager; };
	std::shared_ptr<InputManager> Input() { return m_inputManager; };
	std::shared_ptr<EventManager> Event() { return m_eventManager; };

private:
	void CreateManagers();
	void InitlizeManagers(HWND _hwnd);
};

