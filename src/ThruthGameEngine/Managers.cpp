#include "Managers.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Eventmanager.h"
#include "SceneManager.h"

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

void Managers::Update()
{
	m_inputManager->Update();
	m_timeManager->Update();
	m_eventManager->PublishEvent("Update");
	m_eventManager->Update();
}

void Managers::LateUpdate()
{
	m_eventManager->PublishEvent("Late Update");
	m_eventManager->Update();
}

void Managers::FixedUpdate()
{
}

void Managers::Render()
{
	m_eventManager->PublishEvent("Render");
	m_eventManager->Update();
}

void Managers::Finalize()
{
	m_sceneManager->Finalize();
	m_inputManager->Finalize();
	m_timeManager->Finalize();
	m_eventManager->Finalize();
}

void Managers::CreateManagers()
{
	// TimeManager* temp = new TimeManager();
	m_timeManager = std::make_shared<TimeManager>();
	m_inputManager = std::make_shared<InputManager>();
	m_eventManager = std::make_shared<EventManager>();
	m_sceneManager = std::make_shared<SceneManager>();
}

void Managers::InitlizeManagers(HWND _hwnd)
{
	m_eventManager->Initialize(m_timeManager);
	m_timeManager->Initalize(m_eventManager);
	m_inputManager->Initalize(_hwnd, m_eventManager);
	m_sceneManager->Initalize(m_eventManager);
}
