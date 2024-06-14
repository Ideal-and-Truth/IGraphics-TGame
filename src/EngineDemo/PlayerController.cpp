#include "PlayerController.h"
#include "Transform.h"

PlayerController::PlayerController()
	: m_camera(nullptr)
{
	m_name = "PlayerController";
}

PlayerController::~PlayerController()
{

}

void PlayerController::Awake()
{

}

void PlayerController::Start()
{
	m_camera = m_owner.lock().get()->GetComponent<Truth::Camera>().lock().get();
}

void PlayerController::Update()
{
	auto cameraPos = m_camera->GetTypeInfo().GetProperty("position")->Get<DirectX::SimpleMath::Vector3>(m_camera).Get();
	float speed = 0.1f;
	Vector3 playerPos = m_owner.lock().get()->m_transform->m_position;

	Vector3 direction = playerPos - cameraPos;
	direction.Normalize();
	direction.y = 0;

	Vector3 right = -direction.Cross({ 0.f,1.f,0.f });

	if (GetKey(KEY::W))
	{
		m_owner.lock().get()->m_transform->m_position += direction * speed;
	}
	else if (GetKey(KEY::S))
	{
		m_owner.lock().get()->m_transform->m_position -= direction * speed;
	}

	if (GetKey(KEY::A))
	{
		m_owner.lock().get()->m_transform->m_position -= right * speed;
	}
	else if (GetKey(KEY::D))
	{
		m_owner.lock().get()->m_transform->m_position += right * speed;
	}

	
}
