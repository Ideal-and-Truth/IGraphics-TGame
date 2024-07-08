#pragma once
#include "Component.h"


class StateMachine :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(StateMachine);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:

public:
	StateMachine();
	virtual ~StateMachine();

	METHOD(FixedUpdate);
	void FixedUpdate();

	METHOD(Update);
	void Update();

	METHOD(LateUpdate);
	void LateUpdate();

};

template<class Archive>
void StateMachine::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

