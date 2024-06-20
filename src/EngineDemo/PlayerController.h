#pragma once
#include "Component.h"
#include "Camera.h"


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
	std::shared_ptr<Truth::Camera> m_camera;
	std::shared_ptr<Player> m_player;

	float4 m_forwardInput;
	float4 m_sideInput;

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
	void PlayerMove();
};

template<class Archive>
void PlayerController::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerController)