#pragma once
#include "Component.h"

namespace Truth
{
	class Camera;
	class Controller;
}

class Player;

class PlayerController :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerController);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::Entity> m_camera;
	std::shared_ptr<Truth::Controller> m_controller;
	std::shared_ptr<Player> m_player;

	float4 m_forwardInput;
	float4 m_sideInput;

	float4 m_attackInput;

	Vector3 m_faceDirection;

	PROPERTY(angle);
	Vector3 m_angle;
public:
	PlayerController();
	virtual ~PlayerController();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

private:
	void PlayerMove(const void*);
	void PlayerBattle();

};

template<class Archive>
void PlayerController::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerController)