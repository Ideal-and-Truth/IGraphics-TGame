#include "PlayerCamera.h"
#include "Camera.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerCamera)

PlayerCamera::PlayerCamera()
	: m_camera(nullptr)
	, m_elevation(0.1f)
	, m_azimuth(0.1f)
	, m_cameraDistance(30.f)
	, m_cameraSpeed(0.003f)
	, m_isLockOn(false)
{
	m_name = "PlayerCamera";
}

PlayerCamera::~PlayerCamera()
{

}

void PlayerCamera::Awake()
{

}

void PlayerCamera::Start()
{
	m_camera = m_owner.lock().get()->GetComponent<Truth::Camera>().lock();
	std::vector<std::shared_ptr<Truth::Entity>> entities = m_managers.lock()->Scene()->m_currentScene->GetTypeInfo().GetProperty("entities")->Get<std::vector<std::shared_ptr<Truth::Entity>>>(m_managers.lock()->Scene()->m_currentScene.get()).Get();

	for (auto& e : entities)
	{
		if (e.get()->m_name == "Player")
		{
			m_target = e.get()->m_transform;
		}
	}
	auto ownerPos = m_target->m_position;
	Vector3 targetPos = m_target->m_position;
	targetPos.z -= m_cameraDistance;

	m_owner.lock()->m_transform->m_position = targetPos;

	//m_camera.get()->GetTypeInfo().GetProperty("position")->Set(m_camera.get(), ownerPos);

}

void PlayerCamera::LateUpdate()
{
	if (!m_isLockOn == GetKeyDown(KEY::Q))
	{
		m_isLockOn = true;
	}
	else if (m_isLockOn == GetKeyDown(KEY::Q))
	{
		m_isLockOn = false;
	}

	if (m_isLockOn)
	{
		LockOnCamera();
		return;
	}

	FreeCamera();

}

void PlayerCamera::FreeCamera()
{
	// 자유시점 카메라
	/*Vector3 cameraPos = m_camera.get()->GetTypeInfo().GetProperty("position")->Get<DirectX::SimpleMath::Vector3>(m_camera.get()).Get();
	Vector3 cameraLook = m_camera.get()->GetTypeInfo().GetProperty("look")->Get<DirectX::SimpleMath::Vector3>(m_camera.get()).Get();


	m_elevation += MouseDy() * m_cameraSpeed;
	m_azimuth -= MouseDx() * m_cameraSpeed;

	if (m_elevation > 3.13f)
	{
		m_elevation = 3.13f;
	}
	if (m_elevation < 0.01f)
	{
		m_elevation = 0.01f;
	}


	cameraPos.x = m_cameraDistance * sin(m_elevation) * cos(m_azimuth);
	cameraPos.y = m_cameraDistance * cos(m_elevation);
	cameraPos.z = m_cameraDistance * sin(m_elevation) * sin(m_azimuth);

	auto ownerPos = m_owner.lock()->m_transform->m_position;
	cameraPos.x += ownerPos.x;
	cameraPos.y += ownerPos.y;
	cameraPos.z += ownerPos.z;


	auto look = ownerPos - cameraPos;


	m_camera.get()->GetTypeInfo().GetProperty("position")->Set(m_camera.get(), cameraPos);
	m_camera.get()->GetTypeInfo().GetProperty("look")->Set(m_camera.get(), look);*/

	// 자유시점 카메라2
	Vector3 cameraPos = m_owner.lock()->m_transform->m_position;
	Vector3 targetPos = m_target->m_position;

	m_elevation += MouseDy() * m_cameraSpeed;
	m_azimuth -= MouseDx() * m_cameraSpeed;

	if (m_elevation > 3.13f)
	{
		m_elevation = 3.13f;
	}
	if (m_elevation < 0.01f)
	{
		m_elevation = 0.01f;
	}


	m_cameraDistance -= m_managers.lock()->Input()->m_deltaWheel * 0.01;
	
// 	if (GetKey(KEY::LEFT))
// 	{
// 		m_elevation += m_cameraSpeed;
// 	}
// 	if (GetKey(KEY::RIGHT))
// 	{
// 		m_azimuth -= m_cameraSpeed;
// 	}

	cameraPos.x = m_cameraDistance * sin(m_elevation) * cos(m_azimuth);
	cameraPos.y = m_cameraDistance * cos(m_elevation);
	cameraPos.z = m_cameraDistance * sin(m_elevation) * sin(m_azimuth);

	cameraPos.x += targetPos.x;
	cameraPos.y += targetPos.y;
	cameraPos.z += targetPos.z;

	m_owner.lock()->m_transform->m_position = cameraPos;

	auto look = targetPos - cameraPos;

	// m_camera.get()->GetTypeInfo().GetProperty("look")->Set(m_camera.get(), look);
	m_camera->m_look = look;
}

void PlayerCamera::LockOnCamera()
{

}
