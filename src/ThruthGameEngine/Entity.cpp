#include "Entity.h"
#include "ETransform.h"

uint16 Entity::m_entityCount = 0;

Entity::Entity()
	: m_manager()
	, m_name("Empty Enitity")
	, m_ID(m_entityCount++)
{
}

Entity::~Entity()
{
	m_components.clear();
}

void Entity::Initailize()
{
	AddComponent<ETransform>();
}

