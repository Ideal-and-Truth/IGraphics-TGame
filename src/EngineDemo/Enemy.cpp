#include "Enemy.h"

Enemy::Enemy()
	: m_speed(0.f)
	, m_attackCoefficient(0.f)
	, m_moveCoefficient(0.f)
	, m_maxTP(0.f)
	, m_maxCP(0.f)
	, m_currentDamage(0.f)
	, m_currentTP(0.f)
	, m_currentCP(0.f)
{
	m_name = "Enemy";
}

Enemy::~Enemy()
{

}

void Enemy::Awake()
{

}

void Enemy::Update()
{

}
