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



