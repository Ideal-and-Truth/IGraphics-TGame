#include "Player.h"
#include "Collider.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Player)

Player::Player()
	: m_moveSpeed(3.f)
	, m_lookRotationDampFactor(10.f)
	, m_stopTiming(0.37f)
	, m_maxTP(100.f)
	, m_maxCP(100.f)
	, m_currentDamage(30.f)
	, m_currentTP(0.f)
	, m_currentCP(0.f)
	, m_chargingCP(10.f)
	, m_passingTime(0.f)
	, m_isDecreaseCP(false)
	, m_unlockSkill1(false)
	, m_isInvincible(false)
{
	m_name = "Player";
}

Player::~Player()
{

}

void Player::Awake()
{

}

void Player::Start()
{
	m_currentTP = m_maxTP;
}

void Player::Update()
{
	if (GetKey(KEY::O) && GetKeyDown(KEY::P))
	{
		m_isInvincible = !m_isInvincible;
	}

	if (m_isInvincible)
	{
		m_currentTP = m_maxTP;
		return;
	}

	if (m_currentTP > 0.f)
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime >= 1.f)
		{
			m_currentTP -= 1.f;
			m_passingTime = 0.f;
		}
	}
}

