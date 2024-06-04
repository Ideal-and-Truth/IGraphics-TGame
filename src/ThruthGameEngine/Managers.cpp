#include "Managers.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Eventmanager.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "GraphicsManager.h"

Truth::Managers::Managers()
{
	DEBUG_PRINT("Create Managers\n");
}

Truth::Managers::~Managers()
{

}

void Truth::Managers::Initialize(HWND _hwnd, uint32 _width, uint32 _height)
{
	CreateManagers();
	InitlizeManagers(_hwnd, _width, _height);
}

void Truth::Managers::Update() const
{
	m_inputManager->Update();
	m_timeManager->Update();
	m_physXManager->Update();
	m_sceneManager->Update();
	m_eventManager->Update();
	// m_graphicsManager->Tick();
}

void Truth::Managers::LateUpdate() const
{
	m_eventManager->LateUpdate();
}
 
void Truth::Managers::FixedUpdate() const
{
	m_eventManager->FixedUpdate();
}

void Truth::Managers::Render() const
{
	m_sceneManager->ApplyTransform();
	m_graphicsManager->Render();
}

void Truth::Managers::Finalize() const
{
	m_sceneManager->Finalize();
	m_inputManager->Finalize();
	m_timeManager->Finalize();
	m_eventManager->Finalize();
	m_physXManager->Finalize();
}

void Truth::Managers::CreateManagers()
{
	// TimeManager* temp = new TimeManager();
	m_timeManager = std::make_shared<TimeManager>();
	m_inputManager = std::make_shared<InputManager>();
	m_eventManager = std::make_shared<EventManager>();
	m_sceneManager = std::make_shared<SceneManager>();
	m_physXManager = std::make_shared<PhysicsManager>();
	m_graphicsManager = std::make_shared<GraphicsManager>();
}

void Truth::Managers::InitlizeManagers(HWND _hwnd, uint32 _width, uint32 _height) const
{
	m_eventManager->Initialize(m_timeManager, m_physXManager);
	m_timeManager->Initalize(m_eventManager);
	m_inputManager->Initalize(_hwnd, m_eventManager);
	m_sceneManager->Initalize(m_eventManager);
	m_physXManager->Initalize();
	m_graphicsManager->Initalize(_hwnd, _width, _height);
}
