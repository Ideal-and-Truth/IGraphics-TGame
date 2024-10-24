#include "Enemy.h"
#include "Player.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Enemy)

Enemy::Enemy()
	: m_speed(3.0f)
	, m_attackCoefficient(0.f)
	, m_moveCoefficient(0.f)
	, m_maxTP(100.f)
	, m_maxCP(0.f)
	, m_currentDamage(20.f)
	, m_currentTP(0.f)
	, m_currentCP(0.f)
	, m_stunGuage(0.f)
	, m_attackRange(5.f)
	, m_passingTime(0.f)
	, m_baseSpeed(0.f)
	, m_isTargetIn(false)
	, m_hitOnce(false)
	, m_slowTime(false)
	, m_isInvincible(false)
{
	m_name = "Enemy";
}

Enemy::~Enemy()
{

}

void Enemy::Awake()
{
	m_target = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
}

void Enemy::Start()
{
	m_currentTP = m_maxTP;
	m_baseSpeed = m_speed;
}

void Enemy::Update()
{
	if (GetKey(KEY::O) && GetKeyDown(KEY::P))
	{
		m_isInvincible = !m_isInvincible;
	}

	auto player = m_target.lock()->GetComponent<Player>().lock();
	m_slowTime = player->GetTypeInfo().GetProperty("slowTime")->Get<bool>(player.get()).Get();

	if (m_slowTime)
	{
		m_speed = m_baseSpeed * 0.3f;
		if (player->GetTypeInfo().GetProperty("currentCP")->Get<float>(player.get()).Get() <= 0.f)
		{
			m_speed = m_baseSpeed;
			m_slowTime = false;
		}
	}


	if (m_isInvincible)
	{
		m_currentTP = m_maxTP;
		return;
	}

	if (!m_isTargetIn)
	{
		return;
	}
	if (m_stunGuage > 0.f)
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 1.f)
		{
			m_stunGuage -= 1.f;
			m_passingTime = 0.f;
		}
	}
	else
	{
		m_stunGuage = 0.f;
		m_passingTime = 0.f;
	}

}
