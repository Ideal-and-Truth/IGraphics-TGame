#include "Enemy.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Enemy)

Enemy::Enemy()
	: m_speed(3.0f)
	, m_attackCoefficient(0.f)
	, m_moveCoefficient(0.f)
	, m_maxTP(100.f)
	, m_maxCP(0.f)
	, m_currentDamage(20.f)
	, m_currentTP(100.f)
	, m_currentCP(0.f)
	, m_isTargetIn(false)
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
	
}

void Enemy::Update()
{
	if (!m_isTargetIn)
	{
		return;
	}

}
