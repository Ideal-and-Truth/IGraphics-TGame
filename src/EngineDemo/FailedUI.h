#pragma once
#include "ButtonBehavior.h"

class Player;

class FailedUI
	: public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(FailedUI);

private:
	friend class boost::serialization::access;
	friend class UI;

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

	std::weak_ptr<Player> m_player;

	float m_speed;

	float m_alpha;

public:
	FailedUI();
	virtual ~FailedUI();

private:
	virtual void Start() override;
	virtual void Update() override;
};

template<class Archive>
void FailedUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void FailedUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

BOOST_CLASS_EXPORT_KEY(FailedUI)
BOOST_CLASS_VERSION(FailedUI, 0)