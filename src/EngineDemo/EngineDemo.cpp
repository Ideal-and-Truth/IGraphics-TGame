﻿// EngineDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
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
#include "Player.h"
#include "PlayerCamera.h"
#include "PlayerController.h"
#pragma endregion test Scene

BOOST_CLASS_EXPORT(Truth::RigidBody);
BOOST_CLASS_EXPORT(Truth::Transform);
BOOST_CLASS_EXPORT(Truth::BoxCollider);
BOOST_CLASS_EXPORT(Truth::CapsuleCollider);
BOOST_CLASS_EXPORT(Truth::Collider);
BOOST_CLASS_EXPORT(Truth::Camera);
BOOST_CLASS_EXPORT(Truth::SphereCollider);
BOOST_CLASS_EXPORT(Truth::Mesh);
BOOST_CLASS_EXPORT(Truth::TestComponent);
BOOST_CLASS_EXPORT(Shooter);
BOOST_CLASS_EXPORT(BulletMove);
BOOST_CLASS_EXPORT(Player);
BOOST_CLASS_EXPORT(PlayerCamera);
BOOST_CLASS_EXPORT(PlayerController);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::unique_ptr<Processor> processor = std::make_unique<Processor>();

	processor->Initialize(hInstance);

	std::shared_ptr<Truth::Scene> testScene = std::make_shared<Truth::Scene>(processor->GetManagers());
	testScene->m_name = "test";

	auto wall = std::make_shared<Truth::Entity>(processor->GetManagers());
	wall->m_name = "Wall";
	wall->m_layer = 1;
	testScene->CreateEntity(wall);
	//wall->AddComponent<Truth::RigidBody>();
	//wall->AddComponent<Truth::BoxCollider>(Vector3{ 20.0f, 20.0f, 2.0f }, false);
	wall->AddComponent<Truth::Mesh>(L"debugCube/debugCube");
	wall->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	wall->SetScale(Vector3(40.0f, 1.0f, 40.0f));


	auto shooter = std::make_shared<Truth::Entity>(processor->GetManagers());
	shooter->m_name = "Shooter";
	shooter->m_layer = 1;
	shooter->AddComponent<Shooter>();
	testScene->CreateEntity(shooter);

// 	auto camera = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	camera->m_name = "Camera";
// 	camera->m_layer = 1;
// 	camera->AddComponent<Truth::Camera>()->SetMainCamera();
// 	testScene->CreateEntity(camera);

	auto player = std::make_shared<Truth::Entity>(processor->GetManagers());
	player->m_name = "Player";
	player->m_layer = 1;
	player->AddComponent<PlayerController>();
	player->AddComponent<Truth::Mesh>(L"debugCube/debugCube");
	player->AddComponent<Truth::Camera>()->SetMainCamera();
	player->AddComponent<Player>();
	player->AddComponent<PlayerCamera>();
	player->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
	player->SetScale(Vector3(1.0f, 2.0f, 1.0f));
	testScene->CreateEntity(player);

// 	auto playerCamera = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	playerCamera->m_name = "PlayerCamera";
// 	playerCamera->m_layer = 1;
// 	playerCamera->AddComponent<Truth::Camera>()->SetMainCamera();
// 	playerCamera->AddComponent<PlayerCamera>();
// 	testScene->CreateEntity(playerCamera);

// 	auto triggerWall = std::make_shared<Truth::Entity>(processor->GetManagers());
// 	triggerWall->AddComponent<Truth::BoxCollider>(Vector3{ 20.0f, 20.0f, 2.0f }, true);
// 	triggerWall->AddComponent<Truth::Mesh>(L"debugCube/debugCube");
// 	triggerWall->SetPosition(Vector3(0.0f, 20.0f, 10.0f));
// 	triggerWall->SetScale(Vector3(40.0f, 40.0f, 4.0f));
// 	testScene->CreateEntity(triggerWall);

	processor->AddScene(testScene);

	// processor->LoadScene("../Scene/test.scene");

	processor->SetStartScene("test");
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
