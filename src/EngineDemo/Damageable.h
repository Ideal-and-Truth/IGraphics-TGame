#pragma once
#include "Component.h"

class Damageable :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Damageable);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

public:
	struct DamageMessage
	{
		Truth::Component* damager;
		float amount;
		Vector3 direction;
		Vector3 damageSource;
		bool throwing;
		bool stopCamera;
	};

private:
	PROPERTY(timePoints);
	float4 m_timePoints;

	PROPERTY(invulnerabiltyTime);
	float4 m_invulnerabiltyTime;

	PROPERTY(hitAngle);
	float4 m_hitAngle;
	 
	PROPERTY(hitForwardRotation);
	float4 m_hitForwardRotation;

	PROPERTY(isInvulnerable);
	bool m_isInvulnerable;

public:
	Damageable();
	virtual ~Damageable();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

	void ApplyDamage(DamageMessage InData);

};

template<class Archive>
void Damageable::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

