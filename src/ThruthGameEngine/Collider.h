#pragma once
#include "Component.h"
namespace Truth
{
	class Collider :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Collider)

	public:
		bool m_isTrigger;
		Vector3 m_center;

		Collider(std::shared_ptr<Managers> _managers);
		virtual ~Collider();
	};
}

