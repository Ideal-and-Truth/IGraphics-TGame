#pragma once
#include "Headers.h"
#include "Component.h"

/// <summary>
/// ������ �׽�Ʈ ������Ʈ 
/// </summary>

class TestComponent :
    public Component
{
public:
	TestComponent();
	virtual	~TestComponent();
public:
	void Update(float4 _dt) override;
	void Render() override;
	void LateUpdate(float4 _dt) override;
	void FiexUpdate(float4 _dt) override;

	void EventTestFunc(std::shared_ptr<void> _p);
};

