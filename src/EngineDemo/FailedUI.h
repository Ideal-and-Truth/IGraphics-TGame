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

public:
	FailedUI();
	virtual ~FailedUI();

private:
	virtual void Start() override;
	virtual void Update() override;
};

BOOST_CLASS_EXPORT_KEY(FailedUI)
BOOST_CLASS_VERSION(FailedUI)