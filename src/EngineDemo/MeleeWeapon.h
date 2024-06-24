#pragma once
#include "Component.h"
#include "Collider.h"


class MeleeWeapon :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(MeleeWeapon);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::Collider> m_collider;

public:
	MeleeWeapon();
	virtual ~MeleeWeapon();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();
};

template<class Archive>
void MeleeWeapon::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(MeleeWeapon)