#pragma once
#include "Headers.h"
#include "Component.h"

/// <summary>
/// ������ �׽�Ʈ ������Ʈ 
/// </summary>
namespace Truth
{
	class TestComponent :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(TestComponent)

	public:
		PROPERTY(testInt)
			int m_testInt;

	public:
		TestComponent(std::shared_ptr<Managers> _managers);
		virtual	~TestComponent();

	public:
		METHOD(Update)
			void Update(std::any _p);
		void QUP(std::any _p);
		void QDOWN(std::any _p);
	};
}

