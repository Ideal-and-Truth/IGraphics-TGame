#pragma once
#include "ButtonBehavior.h"

class TitleUI :
    public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(TitleUI);

private:
	friend class boost::serialization::access;
	friend class UI;

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

	float m_alpha;
	float m_speed;
	float m_alphaDirection = 1.0f;

public:
	TitleUI();
	virtual ~TitleUI();

public:
	virtual void Update() override;
	virtual void Start() override;
};

template<class Archive>
void TitleUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void TitleUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

BOOST_CLASS_EXPORT_KEY(TitleUI)
BOOST_CLASS_VERSION(TitleUI, 0)