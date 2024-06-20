#include "PlayerController.h"
#include "Transform.h"
#include "Player.h"

PlayerController::PlayerController()
	: m_camera(nullptr)
	, m_nowSpeed(0.f)
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
	m_camera = m_owner.lock().get()->GetComponent<Truth::Camera>().lock();
	m_player = m_owner.lock().get()->GetComponent<Player>().lock();
}

void PlayerController::Update()
{
	// 플레이어 이동
	PlayerMove();

}

void PlayerController::PlayerMove()
{
	// 플레이어 이동
	Vector3 cameraPos = m_camera.get()->GetTypeInfo().GetProperty("position")->Get<DirectX::SimpleMath::Vector3>(m_camera.get()).Get();

	float maxSpeed = m_player.get()->GetTypeInfo().GetProperty("maxSpeed")->Get<float4>(m_player.get()).Get();

	if (GetKey(KEY::LSHIFT))
	{
		maxSpeed = 3.f;
	}

	Vector3 playerPos = m_owner.lock().get()->m_transform->m_position;

	Vector3 direction = playerPos - cameraPos;

	direction.Normalize();
	direction.y = 0;

	Vector3 right = -direction.Cross({ 0.f,1.f,0.f });

	if (GetKey(KEY::W))
	{
		m_owner.lock().get()->m_transform->m_position += direction * m_nowSpeed;
	}
	else if (GetKey(KEY::S))
	{
		m_owner.lock().get()->m_transform->m_position -= direction * m_nowSpeed;
	}

	if (GetKey(KEY::A))
	{
		m_owner.lock().get()->m_transform->m_position -= right * m_nowSpeed;
	}
	else if (GetKey(KEY::D))
	{
		m_owner.lock().get()->m_transform->m_position += right * m_nowSpeed;
	}


}
