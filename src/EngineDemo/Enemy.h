#pragma once
#include "Component.h"

class Player;

class Enemy :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Enemy);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	PROPERTY(speed);
	float m_speed;

	PROPERTY(attackCoefficient);
	float m_attackCoefficient;

	PROPERTY(moveCoefficient);
	float m_moveCoefficient;

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

	PROPERTY(attackRange);
	float m_attackRange;

	PROPERTY(stunGuage);
	float m_stunGuage;

	PROPERTY(target);
	std::weak_ptr<Truth::Entity> m_target;
	std::weak_ptr<Player> m_player;

	PROPERTY(isTargetIn);
	bool m_isTargetIn;

	PROPERTY(slowTime);
	bool m_slowTime;

	PROPERTY(hitOnce);
	bool m_hitOnce;

	bool m_isInvincible;

	float m_passingTime;

	float m_baseSpeed;

public:
	Enemy();
	virtual ~Enemy();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

	METHOD(LateUpdate);
	void LateUpdate();

	inline float GetMaxTP() const { return m_maxTP; };
	inline float GetCurrentTP() const { return m_currentTP; };
	inline float GetSpeed() const { return m_speed; }
	inline float GetAttackRange() const { return m_attackRange; }

	inline bool GetIsTargetIn() const { return m_isTargetIn; }
	inline bool GetSlowTime() const { return m_slowTime; }
};

template<class Archive>
void Enemy::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

	if (file_version >= 1)
	{
		_ar& m_speed;
		_ar& m_attackCoefficient;
		_ar& m_moveCoefficient;
		_ar& m_maxTP;
		_ar& m_maxCP;
		_ar& m_currentDamage;
		_ar& m_currentTP;
		_ar& m_currentCP;
		_ar& m_attackRange;
	}
}

template<class Archive>
void Enemy::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

	_ar& m_speed;
	_ar& m_attackCoefficient;
	_ar& m_moveCoefficient;
	_ar& m_maxTP;
	_ar& m_maxCP;
	_ar& m_currentDamage;
	_ar& m_currentTP;
	_ar& m_currentCP;
	_ar& m_attackRange;
}


BOOST_CLASS_EXPORT_KEY(Enemy)
BOOST_CLASS_VERSION(Enemy, 1)