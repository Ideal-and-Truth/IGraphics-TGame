#pragma once
#include "Component.h"
class ComponentBase :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(ComponentBase);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:


public:
	ComponentBase();
	virtual ~ComponentBase();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();
};

template<class Archive>
void ComponentBase::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void ComponentBase::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(ComponentBase)
BOOST_CLASS_VERSION(ComponentBase, 0)