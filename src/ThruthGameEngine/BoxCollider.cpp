#include "BoxCollider.h"

Truth::BoxCollider::BoxCollider(std::shared_ptr<Managers> _managers)
	: Collider(_managers)
	, m_size{1.0f, 1.0f, 1.0f}
{
}

Truth::BoxCollider::~BoxCollider()
{
}

void Truth::BoxCollider::Awake()
{

}
