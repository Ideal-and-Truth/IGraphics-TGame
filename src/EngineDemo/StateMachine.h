#pragma once
#include "Component.h"

class State;

class StateMachine :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(StateMachine);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	PROPERTY(currentState);
	State* m_currentState;

public:
	StateMachine();
	virtual ~StateMachine();

	METHOD(FixedUpdate);
	void FixedUpdate();

	METHOD(Update);
	void Update();

	METHOD(LateUpdate);
	void LateUpdate();

	METHOD(SwitchState);
	void SwitchState(State* InState);

};

template<class Archive>
void StateMachine::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

