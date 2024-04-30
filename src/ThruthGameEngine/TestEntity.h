#pragma once
#include "Headers.h"
#include "Entity.h"

/// <summary>
/// 엔티티 클래스 디버깅용 클래스
/// </summary>
class TestEntity
	: public Entity
{
	GENERATE_CLASS_TYPE_INFO(TestEntity)

public:
	TestEntity();
	virtual ~TestEntity();
	virtual void Initailize() override;

	PROPERTY(test)
	std::shared_ptr<int> m_test;
	PROPERTY(intest)
	std::vector<int> m_intest;
};

