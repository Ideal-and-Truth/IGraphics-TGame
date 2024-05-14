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

	public:
		BoxCollider(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner);
		virtual ~BoxCollider();

		void SetSize(Vector3 _size);

	private:
		METHOD(Awake);
		void Awake();
	};
}

