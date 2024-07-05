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
	float4 m_speed;

	PROPERTY(attackCoefficient);
	float4 m_attackCoefficient;

	PROPERTY(moveCoefficient);
	float4 m_moveCoefficient;

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

	PROPERTY(target);
	std::weak_ptr<Truth::Entity> m_target;

	PROPERTY(isTargetIn);
	bool m_isTargetIn;

public:
	Enemy();
	virtual ~Enemy();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();
};

template<class Archive>
void Enemy::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void Enemy::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(Enemy)
BOOST_CLASS_VERSION(Enemy, 0)