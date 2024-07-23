#pragma once
#include "Component.h"
class DamageSystem :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(DamageSystem);
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
	DamageSystem();
	virtual ~DamageSystem();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();
};

template<class Archive>
void DamageSystem::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void DamageSystem::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(DamageSystem)
BOOST_CLASS_VERSION(DamageSystem, 0)