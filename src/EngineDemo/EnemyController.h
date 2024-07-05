#pragma once
#include "Component.h"

namespace Truth
{
	class Controller;
}

class Enemy;

class EnemyController :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(EnemyController);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::weak_ptr<Truth::Controller> m_controller;
	std::weak_ptr<Enemy> m_enemy;
	std::weak_ptr<Truth::Entity> m_target;

public:
	EnemyController();
	virtual ~EnemyController();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();

private:
	void FollowTarget();
};

template<class Archive>
void EnemyController::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void EnemyController::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(EnemyController)
BOOST_CLASS_VERSION(EnemyController, 0)