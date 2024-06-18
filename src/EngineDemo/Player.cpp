#include "Player.h"
#include "Collider.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Player)

Player::Player()
	: m_speed(0.1f)
	, m_jumpForce(10.f)
	, m_lookRotationDampFactor(10.f)
	, m_attackCoefficient(0.1f)
	, m_moveCoefficient(0.1f)
	, m_stopTiming(0.37f)
	, m_maxTP(500.f)
	, m_maxCP(100.f)
	, m_currentDamage(30.f)
	, m_currentTP(500.f)
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

}

