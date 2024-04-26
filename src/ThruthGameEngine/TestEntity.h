#pragma once
#include "Headers.h"
#include "Entity.h"

/// <summary>
/// 엔티티 클래스 디버깅용 클래스
/// </summary>
class TestEntity
	: public Entity
{
public:
	TestEntity();
	virtual ~TestEntity();
	virtual void Initailize() override;

	std::shared_ptr<int> m_test;
	std::vector<int> m_intest;

};

