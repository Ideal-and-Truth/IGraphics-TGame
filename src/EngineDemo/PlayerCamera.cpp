#include "PlayerCamera.h"
#include "Camera.h"
#include "Transform.h"
#include "Enemy.h"
#include "PhysicsManager.h"
#include "Player.h"
#include "PlayerController.h"
#include "EnemyAnimator.h"
#include "BossAnimator.h"
#include "GraphicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerCamera)

PlayerCamera::PlayerCamera()
	: m_elevation(3.3f)
	, m_azimuth(0.1f)
	, m_cameraDistance(5.f)
	, m_cameraSpeed(0.003f)
	, m_passingTime(0.f)
	, m_isLockOn(false)
	, m_isShaking(false)
	// , m_enemyCount(0)
	, m_loopCount(0)
	, m_shakeCount(0.f)
	, m_zoomTime(0.f)
	, m_shakeTime(0.f)
	, m_zoomOutTime(0.f)
	, m_isCutScenePlay(false)
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

	for (auto& e : m_managers.lock()->Scene()->m_currentScene->m_entities)
		if (e->GetComponent<Enemy>().lock())
			m_enemys.push_back(e);
	// 	m_enemys.push_back(m_managers.lock()->Scene()->m_currentScene->FindEntity("RangeEnemy").lock());
	// 	m_enemys.push_back(m_managers.lock()->Scene()->m_currentScene->FindEntity("MeleeEnemy").lock());
	// 	m_enemys.push_back(m_managers.lock()->Scene()->m_currentScene->FindEntity("Boss").lock());

	m_managers.lock()->Graphics()->SetMainCamera(m_camera.lock().get());

	Vector3 rot = m_player.lock()->m_transform->m_rotation.ToEuler();
	rot.x = 0.f;
	rot.z = 0.f;

	m_camera.lock()->m_look = Vector3::Transform(Vector3::Forward, Quaternion::CreateFromYawPitchRoll(rot));

	Vector3 look = m_camera.lock()->m_look;
	m_elevation = acos(m_camera.lock()->m_look.y);
	m_azimuth = acos(m_camera.lock()->m_look.x / sin(m_elevation));
}

void PlayerCamera::FixedUpdate()
{
	if (m_player.lock()->GetComponent<Player>().lock()->GetCurrentTP() <= 0.0f)
	{
		return;
	}
	// if (m_isLockOn && !m_enemys.empty()
	// 	&& !m_player.lock()->GetComponent<Player>().lock()->GetTypeInfo().GetProperty("isDead")->Get<bool>(m_player.lock()->GetComponent<Player>().lock().get()).Get())
	// {
	// 	LockOnCamera();
	// }
	else
	{
		FreeCamera();
	}
}

void PlayerCamera::LateUpdate()
{
	m_playerController->GetTypeInfo().GetProperty("isCutScene")->Set(m_playerController.get(), m_isCutScenePlay);
	m_player.lock()->GetComponent<Player>().lock()->GetTypeInfo().GetProperty("isCutScene")->Set(m_player.lock()->GetComponent<Player>().lock().get(), m_isCutScenePlay);

	if (m_isCutScenePlay)
	{
		return;
	}


	if (m_enemys.empty())
		m_isLockOn = false;

	// if (!m_enemys.empty())
	// {
	// 	if (GetKeyDown(MOUSE::WHEEL))
	// 	{
	// 		m_isLockOn = true;
	// 		m_passingTime = 0.f;
	// 	}
	// 	if (GetKey(MOUSE::WHEEL) && m_isLockOn)
	// 	{
	// 		m_passingTime += GetDeltaTime();
	// 		if (m_isLockOn && m_passingTime > 1.f)
	// 		{
	// 			m_isLockOn = false;
	// 			m_enemyCount = 0;
	// 			m_passingTime = 0.f;
	// 		}
	// 	}
	// 	else if (GetKeyUp(MOUSE::WHEEL) && m_isLockOn)
	// 		m_passingTime = 0.f;
	// }

	/// //////////////////////////////////////////////////////////////////
	/// 락온용
	// if (m_isLockOn && !m_enemys.empty())
	// {
	// 	SortEnemy();
	// 
	// 	// 락온 카메라
	// 	if (GetKeyDown(MOUSE::WHEEL))
	// 	{
	// 		// m_enemyCount++;
	// 
	// 		if (m_enemys.size() <= m_enemyCount)
	// 		{
	// 			m_enemyCount = 0;
	// 		}
	// 	}
	// 
	// 	if (m_enemys.size() <= m_enemyCount)
	// 	{
	// 		m_enemyCount = 0;
	// 		m_loopCount++;
	// 	}
	// 
	// 	if ((m_player.lock()->GetWorldPosition() - m_enemys[m_enemyCount]->GetWorldPosition()).Length() > 20.f)
	// 		m_enemyCount++;
	// 
	// 	if (m_enemys.size() <= m_enemyCount)
	// 	{
	// 		m_enemyCount = 0;
	// 	}
	// 
	// 	if (m_loopCount > 2)
	// 	{
	// 		m_isLockOn = false;
	// 		m_loopCount = 0;
	// 	}
	// 
	// 
	// 	// 락온 중일때 각도 계산
	// 	if (!m_isShaking)
	// 	{
	// 		m_elevation = acos(m_camera.lock()->m_look.y);
	// 		m_azimuth = acos(m_camera.lock()->m_look.x / sin(m_elevation));
	// 	}
	// 
	// 	if (m_camera.lock()->m_look.z < 0.f)
	// 		m_azimuth *= -1.f;
	// }
	/// 자유용
	{
		m_elevation += MouseDy() * m_cameraSpeed;
		m_azimuth -= MouseDx() * m_cameraSpeed;

		if (m_elevation > 3.0f)
			m_elevation = 3.0f;
		if (m_elevation < 0.5f)
			m_elevation = 0.5f;

		if (m_azimuth > 3.14f)
			m_azimuth = -3.14f;
		if (m_azimuth < -3.14f)
			m_azimuth = 3.14f;

		/// TODO : 카메라 줌인 줌아웃 나중에 끄기
		//m_cameraDistance -= m_managers.lock()->Input()->m_deltaWheel * 0.01f;

		if (m_cameraDistance <= 0.0f)
			m_cameraDistance = 0.001f;
	}
	/// //////////////////////////////////////////////////////////////////

	for (auto& e : m_enemys)
	{
		if (e->GetComponent<Enemy>().lock()->GetTypeInfo().GetProperty("currentTP")->Get<float>(e->GetComponent<Enemy>().lock().get()).Get() <= 0.f)
		{
			if (e->GetComponent<BossAnimator>().expired())
			{
				e->Destroy();
			}
			m_enemys.erase(remove(m_enemys.begin(), m_enemys.end(), e));
			// if (m_enemys.size() <= m_enemyCount)
			// {
			// 	m_enemyCount = 0;
			// }
			break;
		}
	}
	if (GetKeyDown(KEY::P))
	{
		//m_isShaking = true;
		m_zoomOutTime = 0.5f;
	}
	if (m_isShaking)
		ShakeCamera(m_shakeCount);

	//ZoomInOut(m_zoomOutTime);
}

void PlayerCamera::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->GetComponent<Enemy>().lock())
		m_enemys.push_back(_other->GetOwner().lock());
}

void PlayerCamera::OnTriggerExit(Truth::Collider* _other)
{
	for (auto& e : m_enemys)
	{
		if (e == _other->GetOwner().lock())
		{
			m_enemys.erase(remove(m_enemys.begin(), m_enemys.end(), e));
			// if (m_enemys.size() <= m_enemyCount)
			// {
			// 	m_enemyCount = 0;
			// }
			break;
		}
	}
}

void PlayerCamera::FreeCamera()
{
	// 자유시점 카메라
	Vector3 cameraPos = m_owner.lock()->m_transform->m_position;
	Vector3 targetPos = m_player.lock()->m_transform->m_position + Vector3{ 0.0f, 1.0f, 0.0f };

	cameraPos.x = sin(m_elevation) * cos(m_azimuth);
	cameraPos.y = cos(m_elevation);
	cameraPos.z = sin(m_elevation) * sin(m_azimuth);

	Vector3 look = cameraPos;
	look.Normalize(look);
	cameraPos = m_managers.lock()->Physics()->GetRayCastHitPoint(targetPos, -look, m_cameraDistance, 1 << 3, 1 << 0);
	// look = targetPos - cameraPos;
	m_owner.lock()->m_transform->m_position = Vector3::Lerp(m_owner.lock()->m_transform->m_position, cameraPos, 0.2f);
	m_camera.lock()->m_look = Vector3::Lerp(m_camera.lock()->m_look, look, 0.2f);

	m_owner.lock()->m_transform->m_rotation = Quaternion::LookRotation(look, Vector3::Up);
	m_owner.lock()->m_transform->m_rotation.z = 0;

}

void PlayerCamera::LockOnCamera()
{
	// if (m_enemys.size() <= m_enemyCount)
	// {
	// 	m_enemyCount = 0;
	// }
	// 
	// Vector3 enemyPos = m_enemys[m_enemyCount]->GetWorldPosition();
	// Vector3 playerPos = m_player.lock()->GetWorldPosition();
	// Vector3 targetPos = playerPos + Vector3{ 0.0f, 2.0f, 0.0f };
	// Vector3 cameraPos = m_owner.lock()->GetWorldPosition();
	// 
	// 
	// Vector3 look = (enemyPos - playerPos - Vector3{ 0.0f, 2.0f, 0.0f });
	// look.Normalize(look);
	// cameraPos = m_managers.lock()->Physics()->GetRayCastHitPoint(targetPos, -look, m_cameraDistance, 1 << 3, 1 << 0);
	// 
	// m_owner.lock()->m_transform->m_position = Vector3::Lerp(m_owner.lock()->m_transform->m_position, cameraPos, 0.2f);
	// m_camera.lock()->m_look = Vector3::Lerp(m_camera.lock()->m_look, look, 0.2f);
	// 
	// m_owner.lock()->m_transform->m_rotation = Quaternion::LookRotation(look, Vector3::Up);
	// m_owner.lock()->m_transform->m_rotation.z = 0.f;


}

void PlayerCamera::SortEnemy()
{
	// for (int i = 0; i < m_enemys.size(); i++)
	// {
	// 	for (int j = 0; j < m_enemys.size(); j++)
	// 	{
	// 		if (i == j || i > j)
	// 			continue;
	// 
	// 		Vector3 playerPos = m_player.lock()->m_transform->m_position;
	// 		Vector3 enemyPos1 = playerPos - m_enemys[i]->m_transform->m_position;
	// 		Vector3 enemyPos2 = playerPos - m_enemys[j]->m_transform->m_position;
	// 
	// 		float enemyDistance1 = (float)sqrt(pow(enemyPos1.x, 2) + pow(enemyPos1.y, 2));
	// 		float enemyDistance2 = (float)sqrt(pow(enemyPos2.x, 2) + pow(enemyPos2.y, 2));
	// 
	// 		if (enemyDistance1 > enemyDistance2)
	// 		{
	// 			std::shared_ptr<Truth::Entity> lastLocked = m_enemys[m_enemyCount];
	// 			m_enemys[j].swap(m_enemys[i]);
	// 			m_enemyCount = (int)(find(m_enemys.begin(), m_enemys.end(), lastLocked) - m_enemys.begin());
	// 			if (m_enemys.size() <= m_enemyCount)
	// 			{
	// 				m_enemyCount = 0;
	// 			}
	// 		}
	// 	}
	// }
}

void PlayerCamera::ShakeCamera(float shakeCount)
{
	m_shakeTime += GetDeltaTime() * 80.f;

	if (m_shakeTime > shakeCount * 6.28f)
	{
		m_shakeTime = 0.f;
		m_shakeCount = 0.f;
		m_isShaking = false;
	}
	m_owner.lock()->m_transform->m_position.z += 0.08f * sin(m_shakeTime);
	m_owner.lock()->m_transform->m_position.y += 0.08f * sin(m_shakeTime);
	m_owner.lock()->m_transform->m_position.x += 0.08f * cos(m_shakeTime);
}

void PlayerCamera::ZoomInOut(float timing)
{
	if (timing > 0.f)
	{
		m_zoomTime += GetDeltaTime();

		if (m_zoomTime > timing)
			m_cameraDistance -= (2.f / m_zoomTime - timing) * GetDeltaTime();
		else
			m_cameraDistance += (2.f / timing) * GetDeltaTime();

		if (m_zoomTime > 2.f)
		{
			m_zoomTime = 0.f;
			m_zoomOutTime = 0.f;
		}
	}
	else
		m_cameraDistance = 10.f;
}

