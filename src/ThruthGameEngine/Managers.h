#pragma once
#include "Headers.h"

class Processor;
class TimeManager;
class InputManager;
class EventManager;
class SceneManager;

class Managers
{
public:
	std::shared_ptr<TimeManager> m_timeManager;
	std::shared_ptr<InputManager> m_inputManager;
	std::shared_ptr<EventManager> m_eventManager;
	std::shared_ptr<SceneManager> m_sceneManager; 

public:
	// friend class Processor;
	Managers();
	~Managers();

public:
	void Initialize(HWND _hwnd);
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void Finalize();

	inline std::shared_ptr<TimeManager> Time() { return m_timeManager; };
	inline std::shared_ptr<InputManager> Input() { return m_inputManager; };
	inline std::shared_ptr<EventManager> Event() { return m_eventManager; };
	inline std::shared_ptr<SceneManager> Scene() { return m_sceneManager; };

private:
	void CreateManagers();
	void InitlizeManagers(HWND _hwnd);
};

