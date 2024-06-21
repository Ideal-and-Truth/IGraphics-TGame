#pragma once
#include "Component.h"

namespace physx
{
	class PxController;
}

namespace Truth
{
	class ControllerTest
		: public Component
	{
		GENERATE_CLASS_TYPE_INFO(ControllerTest);

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	private:
		physx::PxController* m_contorller;

	public:
		ControllerTest();
		~ControllerTest();

		METHOD(Initalize);
		virtual void Initalize() override;

		METHOD(Start);
		virtual void Start() override;

		METHOD(Update);
		virtual void Update() override;
	};

	template<class Archive>
	void Truth::ControllerTest::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}
}
BOOST_CLASS_EXPORT_KEY(Truth::ControllerTest)