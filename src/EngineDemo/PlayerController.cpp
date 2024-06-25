#include "PlayerController.h"
#include "Transform.h"
#include "Camera.h"
#include "Controller.h"
#include "Player.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerController)

PlayerController::PlayerController()
	: m_camera(nullptr)
	, m_controller(nullptr)
	, m_forwardInput(0.f)
	, m_sideInput(0.f)
	, m_attackInput(0.f)
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
	std::vector<std::shared_ptr<Truth::Entity>> entities = m_managers.lock()->Scene()->m_currentScene->GetTypeInfo().GetProperty("entities")->Get<std::vector<std::shared_ptr<Truth::Entity>>>(m_managers.lock()->Scene()->m_currentScene.get()).Get();

	for (auto& e : entities)
	{
		if (e.get()->m_name == "ThirdPersonCamera")
		{
			m_camera = e;
		}
	}

	m_controller = m_owner.lock().get()->GetComponent<Truth::Controller>().lock();
	m_player = m_owner.lock().get()->GetComponent<Player>().lock();

	// �÷��̾� �̵� �̺�Ʈ (�̺�Ʈ�Լ� ���̽�)
	//EventBind("PlayerWalk", &PlayerController::PlayerMove);
}

void PlayerController::Update()
{
	PlayerMove(nullptr);
	// �÷��̾� ����
	PlayerBattle();

}

void PlayerController::PlayerMove(const void*)
{
	// �÷��̾� �̵�
	Vector3 cameraPos = m_camera->m_transform->m_position;

	float playerSpeed = m_player.get()->GetTypeInfo().GetProperty("speed")->Get<float4>(m_player.get()).Get();

	if (GetKey(KEY::LSHIFT))
	{
		playerSpeed *= 2.f;
	}

	Vector3 playerPos = m_owner.lock()->m_transform->m_position;

	Vector3 direction = playerPos - cameraPos;

	direction.Normalize();
	direction.y = 0;


	Vector3 right = -direction.Cross({ 0.f,1.f,0.f });

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

	/// TODO : ������ ��Ʈ�ѷ��� �����ϰ� Ʈ�������� ����� �ٵ�� ����ؾ���

// 	Vector3 disp = direction * m_forwardInput * playerSpeed;
// 	Vector3 disp2 = right * m_sideInput * playerSpeed;
// 	Vector3 gravity = Vector3(0.0f, -100.0f, 0.0f) * GetDeltaTime();
// 	Vector3 finalMovement = disp + disp2 + gravity;
// 	m_controller->Move(finalMovement);

	m_angle = m_camera->m_transform->m_rotation.ToEuler()*(180.f/3.14f);

	Vector3 cameraForward = m_angle;
	Vector3 cameraRight = m_angle * right;
	Vector3 gravity = Vector3(0.0f, -100.0f, 0.0f) * GetDeltaTime();
	cameraForward.y = 0;
	cameraRight.y = 0;
	cameraForward.Normalize();
	cameraRight.Normalize();
	Vector3 disp3 = cameraForward * m_forwardInput * playerSpeed + cameraRight * m_sideInput * playerSpeed;
	Vector3 finalMovement2 = disp3 + gravity;
	m_controller->Move(finalMovement2);
// 	//m_owner.lock()->m_transform->m_position += disp3;


// 	m_owner.lock()->m_transform->m_position += direction * m_forwardInput * playerSpeed;
// 	m_owner.lock()->m_transform->m_position += right * m_sideInput * playerSpeed;

	// �÷��̾� ȸ��
	Vector3 playerDir = direction * m_forwardInput + right * m_sideInput;
	m_faceDirection = { playerDir.x ,0, playerDir.z };
	if (m_faceDirection == Vector3::Zero)
	{
		return;
	}
	Quaternion lookRot;
	Quaternion::LookRotation(m_faceDirection, Vector3::Up, lookRot);
	auto lookRotationDampFactor = m_player.get()->GetTypeInfo().GetProperty("lookRotationDampFactor")->Get<float4>(m_player.get()).Get();
	m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, lookRot, lookRotationDampFactor * GetDeltaTime());
}

void PlayerController::PlayerBattle()
{
	if (GetKey(KEY::LBTN))
	{
		m_attackInput += GetDeltaTime();
	}
	else
	{
		if (m_attackInput > 1.f)
		{
			// ������

			m_attackInput = 0.f;
		}
		else if (m_attackInput < 1.f && m_attackInput > 0.f)
		{
			// �����

			m_attackInput = 0.f;
		}
		else
		{
			m_attackInput = 0.f;
		}
	}


}
