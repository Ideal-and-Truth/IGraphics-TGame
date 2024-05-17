#include "Entity.h"
#include "Transform.h"

uint32 Truth::Entity::m_entityCount = 0;

Truth::Entity::Entity()
	: m_manager()
	, m_name("Empty Enitity")
	, m_ID(m_entityCount++)
	, m_layer(0)
	, m_tag("None")
{
}

Truth::Entity::~Entity()
{
	m_components.clear();
}

void Truth::Entity::Initailize()
{
	AddComponent<Transform>();
}

void Truth::Entity::Awake()
{
	Initailize();
	for (auto& c : m_components)
	{
		auto met = c->GetTypeInfo().GetMethod("Awake");
		if (met)
		{
			met->Invoke<void>(c.get());
		}
	}
	// DEBUG_PRINT(Truth::Entity::StaticTypeInfo().Dump(this).c_str());
}

void Truth::Entity::Destroy()
{
	for (auto& c : m_components)
	{
		auto met = c->GetTypeInfo().GetMethod("Destroy");
		if (met)
		{
			met->Invoke<void>(c.get());
		}
	}
}

void Truth::Entity::Start()
{
	for (auto& c : m_components)
	{
		auto met = c->GetTypeInfo().GetMethod("Start");
		if (met)
		{
			met->Invoke<void>(c.get());
		}
	}
}

void Truth::Entity::OnCollisionEnter(const Collider* _other) const
{
	for (auto& p : m_onCollisionEnter)
	{
		p.second->Invoke<void>(p.first, _other);
	}
}

