#pragma once
#include "Component.h"


class TestEffectGenerator :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(TestEffectGenerator);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

public:

	METHOD(Update);
	void Update();
};


template<class Archive>
void TestEffectGenerator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(TestEffectGenerator)