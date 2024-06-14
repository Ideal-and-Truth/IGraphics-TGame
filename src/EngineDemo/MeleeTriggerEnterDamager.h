#pragma once
#include "Component.h"
class MeleeTriggerEnterDamager :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(MeleeTriggerEnterDamager);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	float4 m_damageAmount;
	bool m_isAttack;

public:
	MeleeTriggerEnterDamager();
	virtual ~MeleeTriggerEnterDamager();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();

	METHOD(OnTriggerEnter);
	void OnTriggerEnter(Truth::Collider* _other);
};

template<class Archive>
void MeleeTriggerEnterDamager::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

