#pragma once
#include "AnimatorController.h"


class PlayerStateMachine :
	public Truth::AnimatorController
{
	GENERATE_CLASS_TYPE_INFO(PlayerStateMachine);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	bool m_isRunning;
	bool m_isAttacking;

private:
	Truth::AnimatorState m_idle;
	Truth::AnimatorState m_run;
	Truth::AnimatorState m_attack;

public:
	PlayerStateMachine();
	virtual ~PlayerStateMachine();

	METHOD(Initalize);
	void Initalize();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();



};
template<class Archive>
void PlayerStateMachine::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void PlayerStateMachine::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}

BOOST_CLASS_EXPORT_KEY(PlayerStateMachine)
BOOST_CLASS_VERSION(PlayerStateMachine, 0)