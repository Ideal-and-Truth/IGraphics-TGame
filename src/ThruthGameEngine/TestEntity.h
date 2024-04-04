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
public:
	void Update(float4 _dt) override;
	void Render() override;
	void LateUpdate(float4 _dt) override;
	void FixedUpdate(float4 _dt) override;
};

