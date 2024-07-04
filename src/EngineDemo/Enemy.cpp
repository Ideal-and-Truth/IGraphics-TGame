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
	, m_target(nullptr)
{
	m_name = "Enemy";
}

Enemy::~Enemy()
{

}

void Enemy::Awake()
{

}

void Enemy::Start()
{
	std::vector<std::shared_ptr<Truth::Entity>> entities = m_managers.lock()->Scene()->m_currentScene->GetTypeInfo().GetProperty("entities")->Get<std::vector<std::shared_ptr<Truth::Entity>>>(m_managers.lock()->Scene()->m_currentScene.get()).Get();

	for (auto& e : entities)
	{
		if (e.get()->m_name == "Player")
		{
			m_target = e;
		}
	}
}

void Enemy::Update()
{
	if (!m_isTargetIn)
	{
		return;
	}

}
