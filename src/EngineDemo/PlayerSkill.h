#pragma once
#include "Component.h"


class PlayerAnimator;

class PlayerSkill :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerSkill);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::shared_ptr<Truth::Entity> m_swordBeam;
	std::shared_ptr<PlayerAnimator> m_playerAnimator;

	float m_swordBeamPos;
	float m_passingTime;

	bool m_useSwordBeam;

	bool m_createComplete;
	bool m_playSwordBeam;

private:
	void SwordBeam();
	void PlayEffect(Vector3 pos);

public:
	PlayerSkill();
	virtual ~PlayerSkill();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();
};

template<class Archive>
void PlayerSkill::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void PlayerSkill::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(PlayerSkill)
BOOST_CLASS_VERSION(PlayerSkill, 0)