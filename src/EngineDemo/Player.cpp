#include "Player.h"
#include "Collider.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Player)

Player::Player()
	: m_moveSpeed(0.01f)
	, m_lookRotationDampFactor(10.f)
	, m_stopTiming(0.37f)
	, m_maxTP(100.f)
	, m_maxCP(100.f)
	, m_currentDamage(30.f)
	, m_currentTP(100.f)
	, m_currentCP(0.f)
	, m_chargingCP(10.f)
	, m_isDecreaseCP(false)
{
	m_name = "Player";
}

Player::~Player()
{

}

void Player::Awake()
{

}

void Player::Update()
{
	if (m_currentTP > 0.f)
	{
		m_currentTP -= GetDeltaTime();
	}
}

