#pragma once
#include "Collider.h"
#include "Headers.h"
namespace Truth
{
	class CapsuleCollider
		: public Collider
	{
		GENERATE_CLASS_TYPE_INFO(CapsuleCollider)

	public:
		float m_radius;
		float m_height;

	public:
		CapsuleCollider(std::shared_ptr<Managers> _managers);
		virtual ~CapsuleCollider();

	};
}

