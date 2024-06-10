#pragma once
#include "Collider.h"
#include "Headers.h"

namespace Truth
{
	class SphereCollider :
		public Collider
	{
		GENERATE_CLASS_TYPE_INFO(SphereCollider);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);
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

	template<class Archive>
	void Truth::SphereCollider::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
		_ar& m_radius;
	}
}

