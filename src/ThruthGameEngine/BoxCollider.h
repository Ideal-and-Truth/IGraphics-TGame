#pragma once
#include "Collider.h"
class BoxCollider :
    public Collider
{
	COMPONENT_HEADER
	GENERATE_CLASS_TYPE_INFO(BoxCollider)

public:
	Vector3 m_size;

public:
	BoxCollider();
	virtual ~BoxCollider();
};

