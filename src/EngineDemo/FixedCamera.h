#pragma once
#include "Camera.h"
class FixedCamera :
    public Truth::Camera
{
	GENERATE_CLASS_TYPE_INFO(FixedCamera)
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

public:
	FixedCamera();
};

template<class Archive>
void FixedCamera::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Truth::Camera>(*this);
	_ar& m_position;
	_ar& m_look;
}

template<class Archive>
void FixedCamera::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Truth::Camera>(*this);
	_ar& m_position;
	_ar& m_look;
}
BOOST_CLASS_EXPORT_KEY(FixedCamera)
BOOST_CLASS_VERSION(FixedCamera, 0)