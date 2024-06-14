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
		GENERATE_CLASS_TYPE_INFO(BoxCollider);

	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	public:
		BoxCollider(bool _isTrigger = true);
		BoxCollider(Vector3 _size, bool _isTrigger = true);
		BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger = true);

		METHOD(Initalize);
		void Initalize();
	};

	template<class Archive>
	void Truth::BoxCollider::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::BoxCollider)