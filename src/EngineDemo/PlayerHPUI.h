#pragma once
#include "ButtonBehavior.h"

class Player;

class PlayerHPUI :
    public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(PlayerHPUI);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::weak_ptr<Player> m_player;

public:
	virtual void Update() override;
	virtual void Start() override;
};

template<class Archive>
void PlayerHPUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void PlayerHPUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}
BOOST_CLASS_EXPORT_KEY(PlayerHPUI)
BOOST_CLASS_VERSION(PlayerHPUI, 0)