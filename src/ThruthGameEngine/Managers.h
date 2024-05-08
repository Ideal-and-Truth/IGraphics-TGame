#pragma once
#include "Headers.h"

namespace Truth
{
	class TimeManager;
	class InputManager;
	class EventManager;
	class SceneManager;
	class PhysicsManager;
	class GraphicsManager;
}

namespace Truth
{

	class Managers
	{
	public:
		std::shared_ptr<Truth::TimeManager> m_timeManager;
		std::shared_ptr<Truth::InputManager> m_inputManager;
		std::shared_ptr<Truth::EventManager> m_eventManager;
		std::shared_ptr<Truth::SceneManager> m_sceneManager;
		std::shared_ptr<Truth::PhysicsManager> m_physXManager;
		std::shared_ptr<Truth::GraphicsManager> m_graphicsManager;

	public:
		Managers();
		~Managers();

	public:
		void Initialize(HWND _hwnd, uint32 _width, uint32 _height);
		void Update() const;
		void LateUpdate() const;
		void FixedUpdate() const;
		void Render() const;

		void Finalize() const;

		inline std::shared_ptr<Truth::TimeManager> Time() const { return m_timeManager; };
		inline std::shared_ptr<Truth::InputManager> Input() const { return m_inputManager; };
		inline std::shared_ptr<Truth::EventManager> Event() const { return m_eventManager; };
		inline std::shared_ptr<Truth::SceneManager> Scene() const { return m_sceneManager; };
		inline std::shared_ptr<Truth::PhysicsManager> Physics() const { return m_physXManager; };
		inline std::shared_ptr<Truth::GraphicsManager> Graphics() const { return m_graphicsManager; };

	private:
		void CreateManagers();
		void InitlizeManagers(HWND _hwnd, uint32 _width, uint32 _height) const;
	};
}

