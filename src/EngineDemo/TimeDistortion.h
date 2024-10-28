#pragma once
#include "Component.h"
#include "Collider.h"

namespace Truth
{
	class SkinnedMesh;
}

class Player;

class TimeDistortion :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(TimeDistortion);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::shared_ptr<Truth::SkinnedMesh> m_playerMesh;
	std::shared_ptr<Player> m_player;
	std::shared_ptr<Truth::Entity> m_playerEntity;

	PROPERTY(active);
	bool m_active;
	PROPERTY(delete);
	bool m_delete;

	Vector3 m_moveVec;

	bool m_isPlayerIn;
	bool m_isFaster;
	int m_count;
	float m_passingTime;
	float m_damage;
	float m_playerSpeed;
	int m_playCount;

private:
	void PlayEffect();

public:
	TimeDistortion();
	virtual ~TimeDistortion();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(FixedUpdate);
	void FixedUpdate();

	METHOD(Update);
	void Update();

	METHOD(OnTriggerEnter);
	void OnTriggerEnter(Truth::Collider* _other);

	METHOD(OnTriggerExit);
	void OnTriggerExit(Truth::Collider* _other);
};

template<class Archive>
void TimeDistortion::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void TimeDistortion::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(TimeDistortion)
BOOST_CLASS_VERSION(TimeDistortion, 0)