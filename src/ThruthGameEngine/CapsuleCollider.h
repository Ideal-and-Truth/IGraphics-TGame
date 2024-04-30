#pragma once
#include "Collider.h"
#include "Headers.h"

class CapsuleCollider
	: public Collider
{
	COMPONENT_HEADER
		GENERATE_CLASS_TYPE_INFO(CapsuleCollider)

public:
	float m_radius;
	float m_height;

public:
	CapsuleCollider();
	virtual ~CapsuleCollider();

};

