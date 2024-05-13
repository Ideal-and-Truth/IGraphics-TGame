#include "Entity.h"
#include "Transform.h"

uint32 Truth::Entity::m_entityCount = 0;

Truth::Entity::Entity()
	: m_manager()
	, m_name("Empty Enitity")
	, m_ID(m_entityCount++)
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
	for (auto c : m_components)
	{
		auto m = c->GetTypeInfo().GetMethod("Awake");
		if (m)
		{
			m->Invoke<void>((void*)c.get());
		}
	}
	DEBUG_PRINT(Truth::Entity::StaticTypeInfo().Dump(this).c_str());
}

void Truth::Entity::Destroy()
{

}

