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
		PROPERTY(radius);
		float m_radius;

	public:
		SphereCollider(bool _isTrigger = true);
		SphereCollider(float _radius, bool _isTrigger = true);
		SphereCollider(Vector3 _pos, float _radius, bool _isTrigger = true);
		virtual ~SphereCollider();

		METHOD(SetRadius);
		void SetRadius(float _radius);

	private:
		METHOD(Awake);
		void Awake();

		METHOD(Initalize);
		void Initalize();
	};
}

