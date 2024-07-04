#pragma once
#include "Component.h"
class Enemy :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Enemy);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

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

	std::shared_ptr<Truth::Entity> m_target;

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
void Enemy::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

