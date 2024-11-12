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
	std::weak_ptr<Truth::Entity> m_camera;
	std::weak_ptr<Truth::Controller> m_controller;
	std::weak_ptr<Player> m_player;

	PROPERTY(forwardInput);
	float m_forwardInput;
	PROPERTY(sideInput);
	float m_sideInput;

	PROPERTY(faceDirection);
	Vector3 m_faceDirection;

	PROPERTY(playerDirection);
	Vector3 m_playerDirection;

	Vector3 m_moveVec;

	PROPERTY(angle);
	Vector3 m_angle;

	PROPERTY(canMove);
	bool m_canMove;

	PROPERTY(impulsePower);
	float m_impulsePower;

	PROPERTY(useImpulse);
	bool m_useImpulse;

	PROPERTY(needRot);
	bool m_needRot;

	PROPERTY(isCutScene);
	bool m_isCutScene;

public:
	PlayerController();
	virtual ~PlayerController();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(FixedUpdate);
	void FixedUpdate();

	METHOD(Update);
	void Update();

private:
	void PlayerMove(const void*);
};

template<class Archive>
void PlayerController::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerController)