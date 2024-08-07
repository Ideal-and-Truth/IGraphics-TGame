#pragma once
#include "Component.h"
class RangedWeapon :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(RangedWeapon);
private:
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:


public:
	RangedWeapon();
	virtual ~RangedWeapon();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();
};

template<class Archive>
void RangedWeapon::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void RangedWeapon::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(RangedWeapon)
BOOST_CLASS_VERSION(RangedWeapon, 0)