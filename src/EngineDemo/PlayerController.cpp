#include "PlayerController.h"
#include "Transform.h"
#include "Camera.h"
#include "Controller.h"
#include "Player.h"
#include "PlayerCamera.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerController)

PlayerController::PlayerController()
	: m_forwardInput(0.f)
	, m_sideInput(0.f)
	, m_canMove(true)
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
	m_camera = m_managers.lock()->Scene()->m_currentScene->FindEntity("ThirdPersonCamera");

	m_controller = m_owner.lock().get()->GetComponent<Truth::Controller>();
	m_player = m_owner.lock().get()->GetComponent<Player>();
}

void PlayerController::Update()
{
	PlayerMove(nullptr);
}

void PlayerController::PlayerMove(const void*)
{
	// 플레이어 이동
	Vector3 cameraPos = m_camera.lock()->m_transform->m_position;

	float playerSpeed = m_player.lock().get()->GetTypeInfo().GetProperty("moveSpeed")->Get<float4>(m_player.lock().get()).Get();

	if (GetKey(KEY::LSHIFT))
	{
		playerSpeed *= 2.f;
	}

	Vector3 playerPos = m_owner.lock()->m_transform->m_position;

	Vector3 direction = playerPos - cameraPos;

	direction.y = 0;
	direction.Normalize();


	Vector3 right = -direction.Cross({ 0.f,1.f,0.f });


	if (!m_canMove)
	{
		m_forwardInput = 0.f;
		m_sideInput = 0.f;
		return;
	}

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


	Vector3 disp = direction * m_forwardInput * playerSpeed;
	Vector3 disp2 = right * m_sideInput * playerSpeed;
	Vector3 gravity = Vector3(0.0f, -100.0f, 0.0f);
	Vector3 finalMovement = (disp + disp2 + gravity) * GetDeltaTime();
	m_controller.lock()->Move(finalMovement);

	// 플레이어 회전
	if (m_camera.lock()->GetComponent<PlayerCamera>().lock()->GetTypeInfo().GetProperty("isLockOn")->Get<bool>(m_camera.lock()->GetComponent<PlayerCamera>().lock().get()).Get())
	{
		Vector3 playerDir = direction;
		m_playerDirection = playerDir;
		Quaternion lookRot;
		Quaternion::LookRotation(playerDir, Vector3::Up, lookRot);
		auto lookRotationDampFactor = m_player.lock().get()->GetTypeInfo().GetProperty("lookRotationDampFactor")->Get<float>(m_player.lock().get()).Get();
		m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, lookRot, lookRotationDampFactor * GetDeltaTime());
	}
	else
	{
		Vector3 playerDir = direction * m_forwardInput + right * m_sideInput;
		m_faceDirection = { playerDir.x ,0, playerDir.z };
		if (m_faceDirection == Vector3::Zero)
		{
			return;
		}
		m_playerDirection = playerDir;
		Quaternion lookRot;
		Quaternion::LookRotation(m_faceDirection, Vector3::Up, lookRot);
		auto lookRotationDampFactor = m_player.lock().get()->GetTypeInfo().GetProperty("lookRotationDampFactor")->Get<float>(m_player.lock().get()).Get();
		m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, lookRot, lookRotationDampFactor * GetDeltaTime());
	}
}
