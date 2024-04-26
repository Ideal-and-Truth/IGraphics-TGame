#pragma once
#include "Headers.h"
#include "Entity.h"

/// <summary>
/// ��ƼƼ Ŭ���� ������ Ŭ����
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

