#pragma once
#include "AnimatorController.h"


class PlayerStates :
	public Truth::AnimatorController
{
	GENERATE_CLASS_TYPE_INFO(PlayerStates);
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
	std::shared_ptr<Truth::AnimatorState> m_idle;
	std::shared_ptr<Truth::AnimatorState> m_run;
	std::shared_ptr<Truth::AnimatorState> m_attack;

	PROPERTY(states);
	std::vector<std::shared_ptr<Truth::AnimatorState>> m_states;

	std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;
public:
	PlayerStates();
	virtual ~PlayerStates();

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
void PlayerStates::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void PlayerStates::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}

BOOST_CLASS_EXPORT_KEY(PlayerStates)
BOOST_CLASS_VERSION(PlayerStates, 0)