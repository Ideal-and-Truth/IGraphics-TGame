#include "PlayerCamera.h"
#include "Camera.h"
#include "Transform.h"
#include "Enemy.h"
#include "PhysicsManager.h"
#include "PlayerController.h"
#include "EnemyAnimator.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerCamera)

PlayerCamera::PlayerCamera()
	: m_elevation(3.3f)
	, m_azimuth(0.1f)
	, m_cameraDistance(10.f)
	, m_cameraSpeed(0.003f)
	, m_passingTime(0.f)
	, m_shakeAzimuth(0.f)
	, m_shakeElevation(0.f)
	, m_isLockOn(false)
	, m_isShaking(false)
	, m_isShaked(false)
	, m_enemyCount(0)
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
	m_camera = m_owner.lock().get()->GetComponent<Truth::Camera>();
	m_player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	Vector3 targetPos = m_player.lock()->m_transform->m_position;
	targetPos.z -= m_cameraDistance;

	m_owner.lock()->m_transform->m_position = targetPos;
	
	m_playerController = m_player.lock()->GetComponent<PlayerController>().lock();

	//m_enemys.push_back(m_managers.lock()->Scene()->m_currentScene->FindEntity("RangerEnemy").lock());
	//m_enemys.push_back(m_managers.lock()->Scene()->m_currentScene->FindEntity("MeleeEnemy").lock());
}

void PlayerCamera::LateUpdate()
{
	if (m_enemys.empty())
	{
		m_isLockOn = false;
	}

	if (m_isLockOn)
	{
		// LockOnCamera();
	}
	else
	{
		FreeCamera();
	}

	if (!m_enemys.empty())
	{
		if (GetKeyDown(MOUSE::WHEEL))
		{
			m_isLockOn = true;
		}
		if (GetKey(MOUSE::WHEEL) && m_isLockOn)
		{
			m_passingTime += GetDeltaTime();
			if (m_isLockOn && m_passingTime > 1.5f)
			{
				m_isLockOn = false;
				m_passingTime = 0.f;
				m_enemyCount = 0;
			}
		}
		else
		{
			m_passingTime = 0.f;
		}
	}


	for (auto& e : m_enemys)
	{
		if (e->GetComponent<Enemy>().lock()->GetTypeInfo().GetProperty("currentTP")->Get<float>(e->GetComponent<Enemy>().lock().get()).Get() <= 0.f)
		{
			m_enemys.erase(remove(m_enemys.begin(), m_enemys.end(), e));
			break;
		}
	}
	if (GetKeyDown(KEY::P))
	{
		//m_isShaking = true;
	}
	if (m_isShaking)
	{
		ShakeCamera();
	}
}

void PlayerCamera::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->GetComponent<Enemy>().lock())
	{
		m_enemys.push_back(_other->GetOwner().lock());
	}
}

void PlayerCamera::OnTriggerExit(Truth::Collider* _other)
{
	for (auto& e : m_enemys)
	{
		if (e == _other->GetOwner().lock())
		{
			m_enemys.erase(remove(m_enemys.begin(), m_enemys.end(), e));
		}
	}
}

void PlayerCamera::FreeCamera()
{
	// 자유시점 카메라
	Vector3 cameraPos = m_owner.lock()->m_transform->m_position;
	Vector3 targetPos = m_player.lock()->m_transform->m_position + Vector3{ 0.0f, 1.0f, 0.0f };

	m_elevation += MouseDy() * m_cameraSpeed;
	m_azimuth -= MouseDx() * m_cameraSpeed;

	if (m_elevation > 3.0f)
	{
		m_elevation = 3.0f;
	}
	if (m_elevation < 0.5f)
	{
		m_elevation = 0.5f;
	}

	if (m_azimuth > 3.14f)
	{
		m_azimuth = -3.14f;
	}
	if (m_azimuth < -3.14f)
	{
		m_azimuth = 3.14f;
	}


	m_cameraDistance -= m_managers.lock()->Input()->m_deltaWheel * 0.01f;
	if (m_cameraDistance <= 0.0f)
	{
		m_cameraDistance = 0.001f;
	}


	cameraPos.x = sin(m_elevation) * cos(m_azimuth);
	cameraPos.y = cos(m_elevation);
	cameraPos.z = sin(m_elevation) * sin(m_azimuth);


	Vector3 look = cameraPos;
	look.Normalize(look);
	cameraPos = m_managers.lock()->Physics()->GetRayCastHitPoint(targetPos, -look, m_cameraDistance);
	// look = targetPos - cameraPos;
	m_owner.lock()->m_transform->m_position = cameraPos;
	m_camera.lock()->m_look = look;

	m_owner.lock()->m_transform->m_rotation = Quaternion::LookRotation(look, Vector3::Up);
	m_owner.lock()->m_transform->m_rotation.z = 0;

}

void PlayerCamera::LockOnCamera()
{
	SortEnemy();

	// 락온 카메라
	if (GetKeyDown(MOUSE::WHEEL))
	{
		m_enemyCount++;
	}
	if (m_enemys.size() <= m_enemyCount)
	{
		m_enemyCount = 0;
	}


	Vector3 enemyPos = m_enemys[m_enemyCount]->m_transform->m_position;
	Vector3 playerPos = m_player.lock()->m_transform->m_position;
	Vector3 targetPos = playerPos + Vector3{ 0.0f, 2.0f, 0.0f };
	Vector3 cameraPos = m_owner.lock()->m_transform->m_position;

	// 락온 중일때 각도 계산
	if (!m_isShaking)
	{
		m_elevation = acos(m_camera.lock()->m_look.y);
		m_azimuth = acos(m_camera.lock()->m_look.x / sin(m_elevation));
	}

	if (m_camera.lock()->m_look.z < 0.f)
	{
		m_azimuth *= -1.f;
	}

	Vector3 look = enemyPos - playerPos - Vector3{ 0.0f, 5.0f, 0.0f };
	look.Normalize(look);
	cameraPos = m_managers.lock()->Physics()->GetRayCastHitPoint(targetPos, -look, m_cameraDistance);

	m_owner.lock()->m_transform->m_position = cameraPos;
	m_camera.lock()->m_look = look;

	m_owner.lock()->m_transform->m_rotation = Quaternion::LookRotation(look, Vector3::Up);
	m_owner.lock()->m_transform->m_rotation.z = 0.f;


}

void PlayerCamera::SortEnemy()
{
	for (int i = 0; i < m_enemys.size(); i++)
	{
		for (int j = 0; j < m_enemys.size(); j++)
		{
			if (i == j || i > j)
			{
				continue;
			}
			Vector3 playerPos = m_player.lock()->m_transform->m_position;
			Vector3 enemyPos1 = playerPos - m_enemys[i]->m_transform->m_position;
			Vector3 enemyPos2 = playerPos - m_enemys[j]->m_transform->m_position;

			float enemyDistance1 = (float)sqrt(pow(enemyPos1.x, 2) + pow(enemyPos1.y, 2));
			float enemyDistance2 = (float)sqrt(pow(enemyPos2.x, 2) + pow(enemyPos2.y, 2));

			if (enemyDistance1 > enemyDistance2)
			{
				std::shared_ptr<Truth::Entity> lastLocked = m_enemys[m_enemyCount];
				m_enemys[j].swap(m_enemys[i]);
				m_enemyCount = (int)(find(m_enemys.begin(), m_enemys.end(), lastLocked) - m_enemys.begin());
			}
		}
	}
}

void PlayerCamera::ShakeCamera()
{
	if (m_shakeAzimuth < 0.3f && !m_isShaked)
	{
		m_shakeAzimuth += 5.f * GetDeltaTime();
		m_shakeElevation += 5.f * GetDeltaTime();
		m_azimuth += 5.f * GetDeltaTime();
		m_elevation += 5.f * GetDeltaTime();
	}
	else if (m_shakeAzimuth >= 0.3f && !m_isShaked)
	{
		m_shakeAzimuth = 0.3f;
		m_shakeElevation = 0.3f;
		m_isShaked = true;
	}

	if (m_isShaked)
	{
		m_shakeAzimuth -= 1.f * GetDeltaTime();
		m_shakeElevation -= 1.f * GetDeltaTime();
		m_azimuth -= 1.f * GetDeltaTime();
		m_elevation -= 1.f * GetDeltaTime();
	}

	if (m_shakeAzimuth <= 0.f)
	{
		m_isShaking = false;
		m_isShaked = false;
	}


}

