#include "Collider.h"

Truth::Collider::Collider(std::shared_ptr<Managers> _managers, bool _isTrigger)
	: Component(_managers)
	, m_isTrigger(_isTrigger)
	, m_center{ 0.0f, 0.0f, 0.0f }
	, m_collider(nullptr)
{
}

Truth::Collider::~Collider()
{
}

