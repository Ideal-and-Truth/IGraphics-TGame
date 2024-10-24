#pragma once
#include "Component.h"
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

	PROPERTY(isTargetIn);
	bool m_isTargetIn;

	float m_passingTime;

public:
	Enemy();
	virtual ~Enemy();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

	inline float GetMaxTP() { return m_maxTP; };
	inline float GetCurrentTP() { return m_currentTP; };
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