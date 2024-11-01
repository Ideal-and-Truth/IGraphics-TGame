#pragma once
#include "ButtonBehavior.h"
class StartButton :
    public Truth::ButtonBehavior
{
    GENERATE_CLASS_TYPE_INFO(StartButton);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

public:
	StartButton();
	virtual void Start() override;
	void OnMouseUp() override;
};

template<class Archive>
void StartButton::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void StartButton::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}
BOOST_CLASS_EXPORT_KEY(StartButton)
BOOST_CLASS_VERSION(StartButton, 0)