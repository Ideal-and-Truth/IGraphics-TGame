#include "Entity.h"
#include "Transform.h"

Entity::Entity()
{
	AddComponent<Transform>();
}

Entity::~Entity()
{

}
