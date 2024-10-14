#pragma once
#include "ButtonBehavior.h"

class ButtonTest :
    public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(ButtonTest);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

public:
	ButtonTest();

	void OnMouseClick() override;
	void OnMouseUp() override;
	void OnMouseOver() override;
};

template<class Archive>
void ButtonTest::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void ButtonTest::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}
BOOST_CLASS_EXPORT_KEY(ButtonTest)
BOOST_CLASS_VERSION(ButtonTest, 0)