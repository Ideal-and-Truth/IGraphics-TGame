#pragma once
#include "Component.h"
#include "Collider.h"

namespace Truth
{
	class Camera;
}

class PlayerController;

class PlayerCamera :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerCamera);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::weak_ptr<Truth::Camera> m_camera;

	std::weak_ptr<Truth::Entity> m_player;

	std::vector<std::shared_ptr<Truth::Entity>> m_enemys;

	std::shared_ptr<PlayerController> m_playerController;

	float m_cameraDistance;
	PROPERTY(elevation);
	float m_elevation;
	PROPERTY(azimuth);
	float m_azimuth;
	float m_cameraSpeed;

	PROPERTY(isLockOn);
	bool m_isLockOn;

	int m_enemyCount;

	float m_passingTime;

public:
	PlayerCamera();
	virtual ~PlayerCamera();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(LateUpdate);
	void LateUpdate();

	METHOD(OnTriggerEnter);
	void OnTriggerEnter(Truth::Collider* _other);

	METHOD(OnTriggerExit);
	void OnTriggerExit(Truth::Collider* _other);

	std::weak_ptr<Truth::Camera> GetCamera() { return m_camera; };

private:
	void FreeCamera();
	void LockOnCamera();
	void SortEnemy();
};

template<class Archive>
void PlayerCamera::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerCamera)