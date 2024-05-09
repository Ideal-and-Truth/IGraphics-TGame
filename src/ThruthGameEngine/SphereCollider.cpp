#include "SphereCollider.h"

Truth::SphereCollider::SphereCollider(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner)
	: Collider(_managers, _owner)
	, m_radius(0.5f)
{

}

Truth::SphereCollider::~SphereCollider()
{

}
