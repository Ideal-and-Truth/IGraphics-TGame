#pragma once
#include "Headers.h"
#include "Component.h"

/// <summary>
/// 디버깅용 테스트 컴포넌트 
/// </summary>
namespace Truth
{
	class TestComponent :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(TestComponent);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	public:
		PROPERTY(testF);
		float m_testF;

		PROPERTY(testV3);
		Vector3 m_testV3;

		PROPERTY(testS)
		std::string m_testS;

	public:
		TestComponent();
		virtual	~TestComponent();

		METHOD(Awake);
		void Awake();

	public:
		METHOD(Update);
		void Update();
	};

	template<class Archive>
	void Truth::TestComponent::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_testF;
	}

}
BOOST_CLASS_EXPORT_KEY(Truth::TestComponent)
