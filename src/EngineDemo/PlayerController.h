#pragma once
#include "Component.h"
#include "Camera.h"


class PlayerController :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerController);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	Truth::Camera* m_camera;

public:
	PlayerController();
	virtual ~PlayerController();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();
};

template<class Archive>
void PlayerController::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

