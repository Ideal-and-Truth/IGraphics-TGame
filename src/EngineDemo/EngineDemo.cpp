// EngineDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

// #pragma warning(push)
// #pragma warning(disable: 26495)
// #pragma warning(disable: 33010)
// #pragma warning(disable: 6297)
// #include <PxPhysicsAPI.h>
// #pragma warning(pop)

#include "framework.h"
#include "EngineDemo.h"
#include "Processor.h"

#pragma region test Scene
#include "Entity.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "Mesh.h"
#include "Shooter.h"
#include "Camera.h"
#include "FreeCamera.h"

#include "Transform.h"
#include "TestComponent.h"
#include "BulletMove.h"

#include "SkinnedMesh.h"
#include "DirectionLight.h"
#include "Animator.h"

#include "Controller.h"

#include "NavMeshGenerater.h"

#include "DirectionLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "UI.h"
#include "TextUI.h"
#pragma endregion test Scene

// #define EDITOR_MODE

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::unique_ptr<Processor> processor = std::make_unique<Processor>();

	processor->Initialize(hInstance);
	///...///
//  	std::shared_ptr<Truth::Scene> testScene = std::make_shared<Truth::Scene>(processor->GetManagers());
//  	testScene->m_name = "test";
//  
// 	auto wall = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	wall->m_name = "Wall";
// 	wall->m_layer = 2;
// 	wall->AddComponent<Truth::DirectionLight>();
// 	testScene->AddEntity(wall);
//  
//  	processor->SaveScene(testScene);

	///...///
 	processor->LoadScene(L"../Scene/test.scene");
	///...///

	processor->Loop();

	return 0;
}
