#pragma once
#include "Camera.h"

namespace Truth
{
	class Camera;
}

class AnchorCamera :
    public Truth::Camera
{
	enum class LOOK_MODE
	{
		FRONT,
		CENTER,
	};

	GENERATE_CLASS_TYPE_INFO(AnchorCamera);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();

	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

public:
	AnchorCamera();
	virtual ~AnchorCamera();

private:
	PROPERTY(anchor);
	Vector3 m_anchor;

	PROPERTY(speed);
	float m_speed;

	PROPERTY(distance);
	float m_distance;

	PROPERTY(height);
	float m_height;

	PROPERTY(azimuth);
	float m_azimuth;

private:

	METHOD(Awake);
	virtual void Awake() override;

	METHOD(Update);
	virtual void Update() override;

};

template<class Archive>
void AnchorCamera::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Camera>(*this);
	_ar& m_anchor;
	_ar& m_speed;
	_ar& m_distance;
	_ar& m_height;
}

template<class Archive>
void AnchorCamera::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Camera>(*this);
	_ar& m_anchor;
	_ar& m_speed;
	_ar& m_distance;
	_ar& m_height;
}


BOOST_CLASS_EXPORT_KEY(AnchorCamera)
BOOST_CLASS_VERSION(AnchorCamera, 0)