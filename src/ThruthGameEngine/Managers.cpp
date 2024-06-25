#include "Managers.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Eventmanager.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "GraphicsManager.h"
#include "EditorCamera.h"
#include "ComponentFactory.h"

Truth::Managers::Managers()
{
	DEBUG_PRINT("Create Managers\n");
}

Truth::Managers::~Managers()
{
	DEBUG_PRINT("Finalize Managers\n");
}

void Truth::Managers::Initialize(HWND _hwnd, uint32 _width, uint32 _height)
{
	CreateManagers();
	InitlizeManagers(_hwnd, _width, _height);
	m_componentFactory = std::make_unique<ComponentFactory>();
#ifdef _DEBUG
	m_editorCamera = std::make_shared<EditorCamera>(this);
	m_editorCamera->SetMainCamera();
#endif // _DEBUG
}

void Truth::Managers::Update() const
{
	m_inputManager->Update();
	m_timeManager->Update();

#ifdef _DEBUG
	if (!m_isEdit)
	{
		m_physXManager->Update();
		m_sceneManager->Update();
		m_eventManager->Update();
	}
	else
	{
		m_editorCamera->SetMainCamera();
		m_editorCamera->Update(m_timeManager->GetDT());
	}
#else
	m_physXManager->Update();
	m_sceneManager->Update();
	m_eventManager->Update();
#endif // _DEBUG

}

void Truth::Managers::LateUpdate() const
{
#ifdef _DEBUG
	if (!m_isEdit)
	{
		m_sceneManager->LateUpdate();
		m_eventManager->LateUpdate();
	}
#else
	m_sceneManager->LateUpdate();
	m_eventManager->LateUpdate();
#endif // _DEBUG
}

void Truth::Managers::FixedUpdate() const
{
	m_physXManager->FixedUpdate();
	m_sceneManager->FixedUpdate();
	m_eventManager->FixedUpdate();
}

void Truth::Managers::Render() const
{
	m_sceneManager->ApplyTransform();

#ifdef _DEBUG
	if (m_isEdit)
	{
		m_graphicsManager->Render();
	}
	else
	{
		m_graphicsManager->CompleteCamera();
		m_graphicsManager->Render();
	}
#else
	m_graphicsManager->Render();
#endif // DEBUG
}

void Truth::Managers::Finalize()
{
	m_sceneManager->Finalize();
	m_sceneManager.reset();

	m_inputManager->Finalize();
	m_inputManager.reset();

	m_timeManager->Finalize();
	m_timeManager.reset();

	m_eventManager->Finalize();
	m_eventManager.reset();

	m_physXManager->Finalize();
	m_physXManager.reset();
}


#ifdef _DEBUG
void Truth::Managers::EditToGame()
{
	if (!m_isEdit)
	{
		return;
	}
	m_sceneManager->SaveCurrentScene();
	m_sceneManager->m_currentScene->Start();
	m_isEdit = false;
}

void Truth::Managers::GameToEdit()
{
	if (m_isEdit)
	{
		return;
	}
	m_physXManager->ResetPhysX();
	m_sceneManager->ReloadSceneData();
	m_isEdit = true;
}
#endif // _DEBUG

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

void Truth::Managers::InitlizeManagers(HWND _hwnd, uint32 _width, uint32 _height)
{
	m_eventManager->Initialize(m_timeManager, m_physXManager);
	m_timeManager->Initalize(shared_from_this());
	m_inputManager->Initalize(_hwnd, m_eventManager);
	m_sceneManager->Initalize(shared_from_this());
	m_physXManager->Initalize();
	m_graphicsManager->Initalize(_hwnd, _width, _height);
}
