#pragma once
#include "Component.h"
class AutoTarget :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(AutoTarget);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:


public:
	AutoTarget();
	virtual ~AutoTarget();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();
};

template<class Archive>
void AutoTarget::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void AutoTarget::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(AutoTarget)
BOOST_CLASS_VERSION(AutoTarget, 0)