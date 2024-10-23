#pragma once
#include "ButtonBehavior.h"

class Enemy;

class BossHPUI :
    public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(BossHPUI);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::weak_ptr<Enemy> m_boss;

public:
	virtual void Update() override;
	virtual void Start() override;
};

template<class Archive>
void BossHPUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void BossHPUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}
BOOST_CLASS_EXPORT_KEY(BossHPUI)
BOOST_CLASS_VERSION(BossHPUI, 0)