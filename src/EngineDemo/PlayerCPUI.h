#pragma once
#include "ButtonBehavior.h"
class Player;

class PlayerCPUI :
    public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(PlayerCPUI);

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
	PlayerCPUI();

	virtual void Update() override;
	virtual void Start() override;
};

template<class Archive>
void PlayerCPUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void PlayerCPUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}
BOOST_CLASS_EXPORT_KEY(PlayerCPUI)
BOOST_CLASS_VERSION(PlayerCPUI, 0)