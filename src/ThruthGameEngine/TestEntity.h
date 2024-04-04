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
public:
	void Update(float4 _dt) override;
	void Render() override;
	void LateUpdate(float4 _dt) override;
	void FixedUpdate(float4 _dt) override;
};

