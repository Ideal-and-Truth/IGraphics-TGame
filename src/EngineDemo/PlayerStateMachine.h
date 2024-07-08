#pragma once
#include "Component.h"


class PlayerStateMachine :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerStateMachine);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	PROPERTY(currentState);
	std::string m_currentState;

	PROPERTY(nextState);
	std::string m_nextState;

public:
	PlayerStateMachine();
	virtual ~PlayerStateMachine();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(FixedUpdate);
	void FixedUpdate();

	METHOD(Update);
	void Update();

	METHOD(LateUpdate);
	void LateUpdate();

	METHOD(SwitchState);
	void SwitchState(std::string InNextState);
};

template<class Archive>
void PlayerStateMachine::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerStateMachine)