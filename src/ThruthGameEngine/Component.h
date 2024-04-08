#pragma once
#include "Headers.h"
#include "EventHandler.h"

/// <summary>
/// ��� ������Ʈ�� �θ� Ŭ����
/// �ϴ��� �̷��� �����غ���...
/// </summary>

class Entity;

class Component
	: public EventHandler
{
private:
	std::string m_name;

protected:
	bool m_canMultiple;
	std::weak_ptr<Entity> m_owner;

public:
	Component();
	virtual ~Component();

	virtual void Update(float4 _dt) abstract;
	virtual void Render() abstract;
	virtual void LateUpdate(float4 _dt) abstract;
	virtual void FiexUpdate(float4 _dt) abstract;

	bool CanMultiple() { return m_canMultiple; }

	std::string GetName() const { return m_name; }
	void SetName(std::string val) { m_name = val; }

	std::weak_ptr<Entity> GetOwner() const { return m_owner; }
	void SetOwner(std::weak_ptr<Entity> val) { m_owner = val; }
};

