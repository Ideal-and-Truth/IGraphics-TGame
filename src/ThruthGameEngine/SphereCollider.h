#pragma once
#include "Collider.h"
#include "Headers.h"

class SphereCollider :
	public Collider
{
	COMPONENT_HEADER
		GENERATE_CLASS_TYPE_INFO(SphereCollider)

public:
	float m_radius;

public:
	SphereCollider();
	virtual ~SphereCollider();
};

