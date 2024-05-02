#include "Collider.h"

Collider::Collider()
	: m_isTrigger(true)
	, m_center{0.0f, 0.0f, 0.0f}
{
}

Collider::~Collider()
{
}

void Collider::Awake()
{
}
