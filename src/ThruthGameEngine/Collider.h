#pragma once
#include "Component.h"
class Collider :
    public Component
{
	COMPONENT_HEADER
	GENERATE_CLASS_TYPE_INFO(Collider)

public:
	bool m_isTrigger;
	Vector3 m_center;
	
	Collider();
	virtual ~Collider();

	virtual void Awake() override;
};

