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
		BoxCollider();
		virtual ~BoxCollider();

	private:
		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();
	};
}

