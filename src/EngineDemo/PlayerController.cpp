#include "PlayerController.h"
#include "Transform.h"
#include "Camera.h"
#include "Controller.h"
#include "Player.h"
#include "PlayerAnimator.h"
#include "PlayerCamera.h"
#include "PhysicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerController)

PlayerController::PlayerController()
	: m_forwardInput(0.f)
	, m_sideInput(0.f)
	, m_impulsePower(0.f)
	, m_useImpulse(true)
	, m_needRot(true)
	, m_canMove(true)
	, m_isCutScene(false)
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
	m_controller.lock()->SetGroup(static_cast<uint32>(Truth::COLLISION_GROUP::PLAYER));
	m_controller.lock()->SetMask(static_cast<uint32>(Truth::COLLISION_GROUP::PLAYER_MASK));
	m_controller.lock()->SetUpFiltering();
	m_player = m_owner.lock().get()->GetComponent<Player>();

	Vector3 rot = m_owner.lock()->m_transform->m_rotation.ToEuler();
	rot.x = 0.f;
	rot.z = 0.f;

	m_playerDirection = Vector3::Transform(Vector3::Forward, Quaternion::CreateFromYawPitchRoll(rot));
}

void PlayerController::FixedUpdate()
{
	if (!m_controller.expired())
	{
		if (!m_canMove || m_isCutScene)
		{
			m_moveVec = Vector3::Zero;
		}
		m_moveVec.y = 0.0f;
		m_moveVec.y = -100.0f;

		m_controller.lock()->Move(m_moveVec);
	}
}

void PlayerController::Update()
{
	if (m_player.lock()->GetTypeInfo().GetProperty("isDead")->Get<bool>(m_player.lock().get()).Get())
	{
		return;
	}

	PlayerMove(nullptr);
}

void PlayerController::PlayerMove(const void*)
{
	// 플레이어 이동
	Vector3 cameraPos = m_camera.lock()->m_transform->m_position;

	float playerSpeed = m_player.lock().get()->GetTypeInfo().GetProperty("moveSpeed")->Get<float>(m_player.lock().get()).Get();


	Vector3 playerPos = m_owner.lock()->m_transform->m_position;

	Vector3 direction = playerPos - cameraPos;

	direction.y = 0;
	direction.Normalize();


	Vector3 right = -direction.Cross({ 0.f,1.f,0.f });

	if (!m_isCutScene)
	{
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

		if (GetKey(KEY::W) && GetKey(KEY::A))
		{
			m_forwardInput = 0.6f;
			m_sideInput = -0.6f;
		}
		else if (GetKey(KEY::W) && GetKey(KEY::D))
		{
			m_forwardInput = 0.6f;
			m_sideInput = 0.6f;
		}

		if (GetKey(KEY::S) && GetKey(KEY::A))
		{
			m_forwardInput = -0.6f;
			m_sideInput = -0.6f;
		}
		else if (GetKey(KEY::S) && GetKey(KEY::D))
		{
			m_forwardInput = -0.6f;
			m_sideInput = 0.6f;
		}
	}


	Vector3 disp = direction * m_forwardInput * playerSpeed;
	Vector3 disp2 = right * m_sideInput * playerSpeed;
	Vector3 gravity = Vector3(0.0f, -100.0f, 0.0f);
	Vector3 finalMovement = (disp + disp2 + gravity);

	Vector3 playerDir = direction * m_forwardInput + right * m_sideInput;
	m_faceDirection = { playerDir.x ,0, playerDir.z };

	/// 임펄스용
	if (m_useImpulse)
	{
		Vector3 impRot;
		if (m_faceDirection == Vector3::Zero)
		{
			Vector3 power(m_playerDirection.x, 0.f, m_playerDirection.z);
			power *= m_impulsePower;
			power *= 0.01f;
			power.y = -100.f;

			m_controller.lock()->AddImpulse(power);

			impRot = m_playerDirection;
		}
		else
		{
			Vector3 power(m_faceDirection.x, 0.f, m_faceDirection.z);
			power *= m_impulsePower;
			power *= 0.01f;
			power.y = -100.f;

			m_controller.lock()->AddImpulse(power);

			impRot = m_faceDirection;
		}
		if (m_needRot)
		{
			Quaternion lookRot;
			Quaternion::LookRotation(impRot, Vector3::Up, lookRot);
			Vector3 euler = lookRot.ToEuler();
			euler.x = 0.f;
			euler.z = 0.f;
			m_owner.lock()->m_transform->m_rotation = Quaternion::CreateFromYawPitchRoll(euler);
		}

		m_needRot = false;
		m_useImpulse = false;
		m_impulsePower = 0.f;
	}


	if (!m_canMove)
	{
		m_moveVec = { 0.f,0.f,0.f };
	}
	m_moveVec = finalMovement;

	/// 락온 중에 공격시 회전
	if (m_camera.lock()->GetComponent<PlayerCamera>().lock()->GetTypeInfo().GetProperty("isLockOn")->Get<bool>(m_camera.lock()->GetComponent<PlayerCamera>().lock().get()).Get())
	{
		auto playerAnimator = m_owner.lock()->GetComponent<PlayerAnimator>().lock();
		if (playerAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(playerAnimator.get()).Get())
		{
			if (m_faceDirection == Vector3::Zero)
			{
				Vector3 playerDir = direction;
				m_playerDirection = playerDir;
				Quaternion lookRot;
				Quaternion::LookRotation(playerDir, Vector3::Up, lookRot);
				Vector3 euler = lookRot.ToEuler();
				euler.x = 0.f;
				euler.z = 0.f;
				auto lookRotationDampFactor = m_player.lock().get()->GetTypeInfo().GetProperty("lookRotationDampFactor")->Get<float>(m_player.lock().get()).Get();
				m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, Quaternion::CreateFromYawPitchRoll(euler), lookRotationDampFactor * GetDeltaTime());
			}
			else
			{
				Quaternion lookRot;
				Quaternion::LookRotation(m_faceDirection, Vector3::Up, lookRot);
				Vector3 euler = lookRot.ToEuler();
				euler.x = 0.f;
				euler.z = 0.f;
				auto lookRotationDampFactor = m_player.lock().get()->GetTypeInfo().GetProperty("lookRotationDampFactor")->Get<float>(m_player.lock().get()).Get();
				m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, Quaternion::CreateFromYawPitchRoll(euler), lookRotationDampFactor * GetDeltaTime());
			}
		}
	}

	if (!m_canMove)
		return;

	/// 락온 중에 가만히 있으면 회전
	if (m_faceDirection == Vector3::Zero)
	{
		if (m_camera.lock()->GetComponent<PlayerCamera>().lock()->GetTypeInfo().GetProperty("isLockOn")->Get<bool>(m_camera.lock()->GetComponent<PlayerCamera>().lock().get()).Get())
		{
			if (!m_camera.lock()->GetComponent<PlayerCamera>().lock()->GetCutScenePaly())
			{
				Vector3 playerDir = direction;
				playerDir.x = 0.f;
				playerDir.z = 0.f;
				m_playerDirection = playerDir;
				Quaternion lookRot;
				Quaternion::LookRotation(playerDir, Vector3::Up, lookRot);
				Vector3 euler = lookRot.ToEuler();
				euler.x = 0.f;
				euler.z = 0.f;
				auto lookRotationDampFactor = m_player.lock().get()->GetTypeInfo().GetProperty("lookRotationDampFactor")->Get<float>(m_player.lock().get()).Get();
				m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, Quaternion::CreateFromYawPitchRoll(euler), lookRotationDampFactor * GetDeltaTime());
			}
		}
		return;
	}

	m_playerDirection = playerDir;
	Quaternion lookRot;
	Quaternion::LookRotation(m_faceDirection, Vector3::Up, lookRot);
	Vector3 euler = lookRot.ToEuler();
	euler.x = 0.f;
	euler.z = 0.f;
	auto lookRotationDampFactor = m_player.lock().get()->GetTypeInfo().GetProperty("lookRotationDampFactor")->Get<float>(m_player.lock().get()).Get();
	m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, Quaternion::CreateFromYawPitchRoll(euler), lookRotationDampFactor * GetDeltaTime());

	// 플레이어 회전
	/*if (m_camera.lock()->GetComponent<PlayerCamera>().lock()->GetTypeInfo().GetProperty("isLockOn")->Get<bool>(m_camera.lock()->GetComponent<PlayerCamera>().lock().get()).Get())
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
	}*/
}
