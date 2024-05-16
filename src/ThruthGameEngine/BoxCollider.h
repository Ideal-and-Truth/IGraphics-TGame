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
		PROPERTY(size);
		Vector3 m_size;

	public:
		BoxCollider(bool _isTrigger = true);
		BoxCollider(Vector3 _size, bool _isTrigger = true);
		BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger = true);
		virtual ~BoxCollider();

		void SetSize(Vector3 _size);

	private:
		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();
	};
}

