#include "Collider.h"

Truth::Collider::Collider(bool _isTrigger)
	: Component()
	, m_isTrigger(_isTrigger)
	, m_center{ 0.0f, 0.0f, 0.0f }
	, m_collider(nullptr)
{
}

Truth::Collider::~Collider()
{
}

