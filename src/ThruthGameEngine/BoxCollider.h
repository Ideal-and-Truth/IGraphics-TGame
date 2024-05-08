#pragma once
#include "Collider.h"

namespace physx
{
	class PxShape;
}

namespace Truth
{
	class BoxCollider :
		public Collider
	{
		GENERATE_CLASS_TYPE_INFO(BoxCollider)

	public:
		Vector3 m_size;

	private:
		physx::PxShape* m_collider;

	public:
		BoxCollider(std::shared_ptr<Managers> _managers);
		virtual ~BoxCollider();
	};
}

