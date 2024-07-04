#pragma once
#include "Component.h"

namespace Truth
{
	class Controller;
}

class TestFollwer :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(TestFollwer);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

	std::weak_ptr<Truth::Controller> m_controller;
	std::weak_ptr<Truth::Entity> m_player;

public:
	TestFollwer();
	~TestFollwer();

	METHOD(Update);
	virtual void Update() override;

	METHOD(Awake);
	virtual void Awake() override;
};


template<class Archive>
void TestFollwer::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Truth::Component>(*this);
}

template<class Archive>
void TestFollwer::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Truth::Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(TestFollwer)
BOOST_CLASS_VERSION(TestFollwer, 0)