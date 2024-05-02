#pragma once
#include "Headers.h"

class Processor;
class TimeManager;
class InputManager;
class EventManager;
class SceneManager;
class PhysicsManager;

class Managers
{
public:
	std::shared_ptr<TimeManager> m_timeManager;
	std::shared_ptr<InputManager> m_inputManager;
	std::shared_ptr<EventManager> m_eventManager;
	std::shared_ptr<SceneManager> m_sceneManager;
	std::shared_ptr<PhysicsManager> m_physXManager;

public:
	// friend class Processor;
	Managers();
	~Managers();

public:
	void Initialize(HWND _hwnd) ;
	void Update() const;
	void LateUpdate() const;
	void FixedUpdate() const;
	void Render() const;

	void Finalize() const;

	inline std::shared_ptr<TimeManager> Time() const  { return m_timeManager; };
	inline std::shared_ptr<InputManager> Input() const  { return m_inputManager; };
	inline std::shared_ptr<EventManager> Event() const  { return m_eventManager; };
	inline std::shared_ptr<SceneManager> Scene() const  { return m_sceneManager; };

private:
	void CreateManagers();
	void InitlizeManagers(HWND _hwnd) const;
};

