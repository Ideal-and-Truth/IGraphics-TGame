#pragma once
#include "CineCamera.h"

class PlayerCamera;

class CutSceneTrigger :
	public Truth::CineCamera
{
	GENERATE_CLASS_TYPE_INFO(CutSceneTrigger);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

public:
	CutSceneTrigger();

private:
	std::weak_ptr<PlayerCamera> m_pCamera;

protected:
	METHOD(OnTriggerEnter);
	virtual void OnTriggerEnter(Truth::Collider* _other) override;

	virtual void Update() override;
};

template<class Archive>
void CutSceneTrigger::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Truth::CineCamera>(*this);
}

template<class Archive>
void CutSceneTrigger::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Truth::CineCamera>(*this);
}
BOOST_CLASS_EXPORT_KEY(CutSceneTrigger)
BOOST_CLASS_VERSION(CutSceneTrigger, 0)