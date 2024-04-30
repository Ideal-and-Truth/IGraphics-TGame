#pragma once
#include "Headers.h"
#include "Entity.h"

/// <summary>
/// ��ƼƼ Ŭ���� ������ Ŭ����
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

