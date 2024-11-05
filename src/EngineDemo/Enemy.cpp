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
	m_player = m_target.lock()->GetComponent<Player>();
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
	if (GetKey(KEY::_9))
	{
		m_isTargetIn = true;
	}



	if (m_isInvincible)
	{
		m_currentTP = m_maxTP;
		return;
	}

	if (!m_isTargetIn)
		return;
}

void Enemy::LateUpdate()
{
	std::shared_ptr<Player> player = m_player.lock();

	m_slowTime = player->GetSlowTime();

	if (m_slowTime && m_owner.lock()->m_name != "Boss")
	{
		m_speed = m_baseSpeed * 0.3f;
		if (player->GetCurrentCP() <= 0.f)
		{
			m_speed = m_baseSpeed;
			m_slowTime = false;
		}
	}
}
