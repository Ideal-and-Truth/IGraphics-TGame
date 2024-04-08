#include "Managers.h"

void Managers::Initialize(HWND _hwnd)
{
	CreateManagers();
	InitlizeManagers(_hwnd);
}

void Managers::Update()
{
	m_timeManager->Update();
	m_inputManager->Update();
	m_eventManager->Update(m_timeManager->GetDT());
}

void Managers::CreateManagers()
{
	m_timeManager = std::make_shared<TimeManager>();
	m_inputManager = std::make_shared<InputManager>();
	m_eventManager = std::make_shared<EventManager>();
}

void Managers::InitlizeManagers(HWND _hwnd)
{
	m_eventManager->Initialize();
	m_timeManager->Initalize();
	m_inputManager->Initalize(_hwnd);
}
