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
#include "Mesh.h"
#include "Shooter.h"
#include "Camera.h"

#include "Transform.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "TestComponent.h"
#include "BulletMove.h"

#include "SkinnedMesh.h"
#include "Light.h"

#include "Controller.h"
#pragma endregion test Scene

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
// 	std::shared_ptr<Truth::Scene> testScene = std::make_shared<Truth::Scene>(processor->GetManagers());
// 	testScene->m_name = "test";
// 
// 	auto wall = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	wall->m_name = "Wall";
// 	wall->m_layer = 2;
// 	wall->AddComponent<Truth::RigidBody>();
// 	wall->AddComponent<Truth::BoxCollider>(false);
// 	wall->AddComponent<Truth::Mesh>(L"DebugObject/debugCube");
// 	wall->SetPosition(Vector3(0.0f, 20.0f, 0.0f));
// 	wall->SetScale(Vector3(40.0f, 40.0f, 4.0f));
// 	testScene->AddEntity(wall);
// 
// 	auto shooter = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	shooter->m_name = "Shooter";
// 	shooter->m_layer = 1;
// 	shooter->AddComponent<Shooter>();
// 	testScene->AddEntity(shooter);
// 
// 	auto camera = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	camera->m_name = "Camera";
// 	camera->m_layer = 1;
// 	camera->AddComponent<Truth::Camera>()->SetMainCamera();
// 	testScene->AddEntity(camera);
// 
// 	auto triggerWall = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	triggerWall->m_name = "TriggerWall";
// 	triggerWall->AddComponent<Truth::BoxCollider>(true);
// 	triggerWall->AddComponent<Truth::Mesh>(L"DebugObject/debugCube");
// 	triggerWall->SetPosition(Vector3(0.0f, 20.0f, 10.0f));
// 	triggerWall->SetScale(Vector3(40.0f, 40.0f, 4.0f));
// 	testScene->AddEntity(triggerWall);
// 
// 	processor->SaveScene(testScene);

	///...///
 	processor->LoadScene(L"../Scene/test.scene");

 	//processor->LoadScene(L"../Scene/ControllerTest.scene");


	///...///

	processor->Loop();

// 	std::ifstream inputstream("../data");
// 	boost::archive::text_iarchive inputArchive(inputstream);
// 	Truth::Scene* s(nullptr);
// 	inputArchive >> s;
// 
// 	int a = 1;

	return 0;
}
