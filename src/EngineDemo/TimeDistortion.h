#pragma once
#include "Component.h"
#include "Collider.h"

namespace Truth
{
	class SkinnedMesh;
}

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

	bool m_isPlayerIn;
	bool m_isFaster;

public:
	TimeDistortion();
	virtual ~TimeDistortion();

	METHOD(Awake);
	void Awake();

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