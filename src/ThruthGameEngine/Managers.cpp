#include "Managers.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Eventmanager.h"
#include "SceneManager.h"
#include "PhysicsManager.h"

Managers::Managers()
{
	DEBUG_PRINT("Create Managers\n");
}

Managers::~Managers()
{

}

void Managers::Initialize(HWND _hwnd)
{
	CreateManagers();
	InitlizeManagers(_hwnd);
}

void Managers::Update() const
{
	m_inputManager->Update();
	m_timeManager->Update();
	m_eventManager->PublishEvent("Update");
	m_eventManager->Update();
}

void Managers::LateUpdate() const
{
	m_eventManager->PublishEvent("Late Update");
	m_eventManager->Update();
}
 
void Managers::FixedUpdate() const
{
}

void Managers::Render() const
{
	m_eventManager->PublishEvent("Render");
	m_eventManager->Update();
}

void Managers::Finalize() const
{
	m_sceneManager->Finalize();
	m_inputManager->Finalize();
	m_timeManager->Finalize();
	m_eventManager->Finalize();
	m_physXManager->Finalize();
}

void Managers::CreateManagers()
{
	// TimeManager* temp = new TimeManager();
	m_timeManager = std::make_shared<TimeManager>();
	m_inputManager = std::make_shared<InputManager>();
	m_eventManager = std::make_shared<EventManager>();
	m_sceneManager = std::make_shared<SceneManager>();
	m_physXManager = std::make_shared<PhysicsManager>();
}

void Managers::InitlizeManagers(HWND _hwnd) const
{
	m_eventManager->Initialize(m_timeManager);
	m_timeManager->Initalize(m_eventManager);
	m_inputManager->Initalize(_hwnd, m_eventManager);
	m_sceneManager->Initalize(m_eventManager);
	m_physXManager->Initalize();
}
