#pragma once
#include "Headers.h"
#include "Component.h"

/// <summary>
/// ������ �׽�Ʈ ������Ʈ 
/// </summary>

class TestComponent :
	public Component
{
	COMPONENT_HEADER;

public:
	TestComponent();
	virtual	~TestComponent();
	virtual void Awake() override;

public:
	void Update(std::any _p);
	void QUP(std::any _p);
	void QDOWN(std::any _p);

	REFLECT()
};

