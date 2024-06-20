#pragma once
#include "Component.h"
class ComponentBase :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(ComponentBase);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

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
void ComponentBase::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

