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
		PROPERTY(testInt);
		int m_testInt;

	public:
		TestComponent();
		virtual	~TestComponent();

		METHOD(Awake);
		void Awake();

	public:
		METHOD(Update);
		void Update();

		void QUP(const void* _p);
		void QDOWN(const void* _p);
	};

	template<class Archive>
	void Truth::TestComponent::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}

}
BOOST_CLASS_EXPORT_KEY(Truth::TestComponent)
