#pragma once
#include "Component.h"


class Player :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Player);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	PROPERTY(moveSpeed);
	float m_moveSpeed;

	PROPERTY(lookRotationDampFactor);
	float m_lookRotationDampFactor;

	PROPERTY(stopTiming);
	float m_stopTiming;

	PROPERTY(maxTP);
	float m_maxTP;

	PROPERTY(maxCP);
	float m_maxCP;

	PROPERTY(currentDamage);
	float m_currentDamage;

	PROPERTY(currentTP);
	float m_currentTP;

	PROPERTY(currentCP);
	float m_currentCP;

	PROPERTY(chargingCP);
	float m_chargingCP;

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

BOOST_CLASS_EXPORT_KEY(Player)