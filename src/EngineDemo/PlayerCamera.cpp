#include "PlayerCamera.h"
#include "Camera.h"
#include "Transform.h"

PlayerCamera::PlayerCamera()
	: m_camera(nullptr)
	, m_elevation(0.1f)
	, m_azimuth(0.1f)
	, m_cameraDistance(30.f)
	, m_cameraSpeed(0.003f)
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
	m_camera = m_owner.lock().get()->GetComponent<Truth::Camera>().lock().get();

	auto ownerPos = m_owner.lock().get()->m_transform->m_position;
	ownerPos.z -= m_cameraDistance;
	m_camera->GetTypeInfo().GetProperty("position")->Set(m_camera, ownerPos);

}

void PlayerCamera::Update()
{
	auto cameraPos = m_camera->GetTypeInfo().GetProperty("position")->Get<DirectX::SimpleMath::Vector3>(m_camera).Get();
	auto cameraLook = m_camera->GetTypeInfo().GetProperty("look")->Get<DirectX::SimpleMath::Vector3>(m_camera).Get();


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

	auto ownerPos = m_owner.lock().get()->m_transform->m_position;
	cameraPos.x += ownerPos.x;
	cameraPos.y += ownerPos.y;
	cameraPos.z += ownerPos.z;


	auto look = ownerPos - cameraPos;


	m_camera->GetTypeInfo().GetProperty("position")->Set(m_camera, cameraPos);
	m_camera->GetTypeInfo().GetProperty("look")->Set(m_camera, look);

	m_camera->Update();




}
