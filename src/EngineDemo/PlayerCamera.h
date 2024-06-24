#pragma once
#include "Component.h"

namespace Truth
{
	class Camera;
}

class PlayerCamera :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerCamera);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::Camera> m_camera;

	std::shared_ptr<Truth::Transform> m_target;

	float4 m_cameraDistance;
	PROPERTY(elevation);
	float4 m_elevation;
	PROPERTY(azimuth);
	float4 m_azimuth;
	float4 m_cameraSpeed;

	bool m_isLockOn;

public:
	PlayerCamera();
	virtual ~PlayerCamera();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

private:
	void FreeCamera();
	void LockOnCamera();
};

template<class Archive>
void PlayerCamera::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerCamera)