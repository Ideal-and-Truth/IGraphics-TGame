#include "SphereCollider.h"

Truth::SphereCollider::SphereCollider(std::shared_ptr<Managers> _managers)
	: Collider(_managers)
	, m_radius(0.5f)
{

}

Truth::SphereCollider::~SphereCollider()
{

}
