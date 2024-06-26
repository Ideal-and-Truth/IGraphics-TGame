#pragma once
#include "Component.h"
class DamageSystem :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(DamageSystem);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

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
void DamageSystem::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(DamageSystem)