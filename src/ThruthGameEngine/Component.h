#pragma once
#include "Headers.h"

/// <summary>
/// 모든 컴포넌트의 부모 클래스
/// 일단은 이렇게 구현해보자...
/// </summary>

class Entity;

class Component
{
private:
	bool m_canMultiple;
	std::weak_ptr<Entity> m_owner;

public:
	Component();
	virtual ~Component();

	virtual void Update(float4 _dt) abstract;
	virtual void Render() abstract;
	virtual void LateUpdate(float4 _dt) abstract;
	virtual void FiexUpdate(float4 _dt) abstract;

	bool CanMultiple() const { return m_canMultiple; }
	void CanMultiple(bool val) { m_canMultiple = val; }
};

