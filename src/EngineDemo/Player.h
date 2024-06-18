#pragma once
#include "Component.h"

class Truth::Collider;

class Player :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Player);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	PROPERTY(maxSpeed);
	float4 m_maxSpeed;

	PROPERTY(jumpForce);
	float4 m_jumpForce;

	PROPERTY(lookRotationDampFactor);
	float4 m_lookRotationDampFactor;

	PROPERTY(attackCoefficient);
	float4 m_attackCoefficient;

	PROPERTY(moveCoefficient);
	float4 m_moveCoefficient;

	PROPERTY(stopTiming);
	float4 m_stopTiming;

	PROPERTY(maxTP);
	float4 m_maxTP;

	PROPERTY(maxCP);
	float4 m_maxCP;

	PROPERTY(currentDamage);
	float4 m_currentDamage;

	PROPERTY(currentTP);
	float4 m_currentTP;

	PROPERTY(currentCP);
	float4 m_currentCP;

	PROPERTY(chargingCP);
	float4 m_chargingCP;

	PROPERTY(isDecreaseCP);
	bool m_isDecreaseCP;

public:
	Player();
	virtual ~Player();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();

private:
};

template<class Archive>
void Player::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

