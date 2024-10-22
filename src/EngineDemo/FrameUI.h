#pragma once
#include "ButtonBehavior.h"

namespace Truth
{
	class TimeManager;
}

class FrameUI :
	public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(FrameUI);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);


	float m_interval;
	float m_adt;
	uint32 m_frameCount;

public:
	FrameUI();

	void Update() override;
	void Start() override;
};

template<class Archive>
void FrameUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void FrameUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}
BOOST_CLASS_EXPORT_KEY(FrameUI)
BOOST_CLASS_VERSION(FrameUI, 0)