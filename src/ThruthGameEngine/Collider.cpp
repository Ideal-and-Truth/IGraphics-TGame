#include "Collider.h"

Truth::Collider::Collider(std::shared_ptr<Managers> _managers)
	: Component(_managers)
	, m_isTrigger(true)
	, m_center{ 0.0f, 0.0f, 0.0f }
{
}

Truth::Collider::~Collider()
{
}

void Truth::Collider::Awake()
{
}
