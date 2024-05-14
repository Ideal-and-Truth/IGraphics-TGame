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
		GENERATE_CLASS_TYPE_INFO(TestComponent)

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
		void Update(std::any _p);

		void QUP(std::any _p);
		void QDOWN(std::any _p);
	};
}

