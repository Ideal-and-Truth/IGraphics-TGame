#include "PlayerController.h"
#include "Transform.h"
#include "Player.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerController)

PlayerController::PlayerController()
	: m_camera(nullptr)
	, m_forwardInput(0.f)
	, m_sideInput(0.f)
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

	float playerSpeed = m_player.get()->GetTypeInfo().GetProperty("speed")->Get<float4>(m_player.get()).Get();

	if (GetKey(KEY::LSHIFT))
	{
		playerSpeed *= 2.f;
	}

	Vector3 playerPos = m_owner.lock().get()->m_transform->m_position;

	Vector3 direction = playerPos - cameraPos;

	direction.Normalize();
	direction.y = 0;

	Vector3 right = -direction.Cross({ 0.f,1.f,0.f }) * m_sideInput;

	if (GetKey(KEY::W))
	{
		m_forwardInput = 1.f;
	}
	else if (GetKey(KEY::S))
	{
		m_forwardInput = -1.f;
	}
	else
	{
		m_forwardInput = 0.f;
	}
	if (GetKey(KEY::W) && GetKey(KEY::S))
	{
		m_forwardInput = 0.f;
	}


	if (GetKey(KEY::A))
	{
		m_sideInput = -1.f;
	}
	else if (GetKey(KEY::D))
	{
		m_sideInput = 1.f;
	}
	else
	{
		m_sideInput = 0.f;
	}
	if (GetKey(KEY::A) && GetKey(KEY::D))
	{
		m_sideInput = 0.f;
	}

	m_owner.lock().get()->m_transform->m_position += direction * m_forwardInput * playerSpeed;
	m_owner.lock().get()->m_transform->m_position += right * playerSpeed;


}
