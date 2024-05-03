#pragma once
#include "Collider.h"
#include "Headers.h"
namespace Truth
{
	class SphereCollider :
		public Collider
	{
		GENERATE_CLASS_TYPE_INFO(SphereCollider)

	public:
		float m_radius;

	public:
		SphereCollider();
		virtual ~SphereCollider();
	};
}

