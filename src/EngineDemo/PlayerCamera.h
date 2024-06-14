#pragma once
#include "Component.h"
#include "Camera.h"


class PlayerCamera :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerCamera);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	Truth::Camera* m_camera;

	float4 m_cameraDistance;
	float4 m_elevation;
	float4 m_azimuth;
	float4 m_cameraSpeed;

public:
	PlayerCamera();
	virtual ~PlayerCamera();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();
};

template<class Archive>
void PlayerCamera::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

