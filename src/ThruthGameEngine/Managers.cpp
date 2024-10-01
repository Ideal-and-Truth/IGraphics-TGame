#include "Managers.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Eventmanager.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "GraphicsManager.h"
#include "EditorCamera.h"
#include "ComponentFactory.h"
#include <time.h>

Truth::Managers::Managers()
{
	DEBUG_PRINT("Create Managers\n");
}

Truth::Managers::~Managers()
{
	DEBUG_PRINT("Finalize Managers\n");
}

void Truth::Managers::Initialize(HINSTANCE _hinstance, HWND _hwnd, uint32 _width, uint32 _height)
{
	CreateManagers();
	InitlizeManagers(_hinstance, _hwnd, _width, _height);
	m_componentFactory = std::make_unique<ComponentFactory>();
#ifdef EDITOR_MODE
	m_editorCamera = std::make_shared<EditorCamera>(this);
	m_editorCamera->SetMainCamera();
#endif // EDITOR_MODE
}

void Truth::Managers::Update() const
{
	m_inputManager->Update();
	m_timeManager->Update();

#ifdef EDITOR_MODE
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
#endif // EDITOR_MODE

}

void Truth::Managers::LateUpdate() const
{
#ifdef EDITOR_MODE
	if (!m_isEdit)
	{
		m_sceneManager->LateUpdate();
		m_eventManager->LateUpdate();
	}
#else
	m_sceneManager->LateUpdate();
	m_eventManager->LateUpdate();
#endif // EDITOR_MODE
}

void Truth::Managers::FixedUpdate() const
{
	m_physXManager->FixedUpdate();
	m_sceneManager->FixedUpdate();
	m_eventManager->FixedUpdate();
}

void Truth::Managers::Render() const
{
	clock_t start, finish;
	double duration;

	start = clock();

	m_sceneManager->ApplyTransform();
	finish = clock();

	std::string temp = std::to_string(finish - start);
	temp = std::string("update : ") + temp;
	temp += " / ";
	DEBUG_PRINT(temp.c_str());

	start = clock();
#ifdef EDITOR_MODE
	if (m_isEdit)
	{
		m_graphicsManager->Render();
	}
	else
	{
		// m_graphicsManager->CompleteCamera();
		m_graphicsManager->Render();
	}
#else
	m_graphicsManager->Render();
#endif // EDITOR_MODE
	finish = clock();
	temp = std::to_string(finish - start);
	temp = std::string("render : ") + temp;
	temp += " \n ";
	DEBUG_PRINT(temp.c_str());

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

	m_graphicsManager->Finalize();
	m_graphicsManager.reset();
}


#ifdef EDITOR_MODE
void Truth::Managers::EditToGame()
{
	if (!m_isEdit)
	{
		return;
	}
	m_sceneManager->SaveCurrentScene();
	m_sceneManager->m_currentScene->Start();
	m_isEdit = false;
	m_inputManager->m_fpsMode = true;
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
	m_inputManager->m_fpsMode = false;
}
#endif // EDITOR_MODE

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

void Truth::Managers::InitlizeManagers(HINSTANCE _hinstance, HWND _hwnd, uint32 _width, uint32 _height)
{
	m_eventManager->Initialize(m_timeManager, m_physXManager);
	m_timeManager->Initalize(shared_from_this());
	m_inputManager->Initalize(_hinstance, _hwnd, m_eventManager);
	m_sceneManager->Initalize(shared_from_this());
	m_physXManager->Initalize();
	m_graphicsManager->Initalize(_hwnd, _width, _height);
}
