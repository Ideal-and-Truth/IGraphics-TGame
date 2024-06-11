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
		PROPERTY(size);
		Vector3 m_size;

	public:
		BoxCollider(bool _isTrigger = true);
		BoxCollider(Vector3 _size, bool _isTrigger = true);
		BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger = true);
		virtual ~BoxCollider();

		METHOD(SetSize);
		void SetSize(Vector3 _size);

		METHOD(Initalize);
		void Initalize();

	private:
		METHOD(Awake);
		void Awake();
	};

	template<class Archive>
	void Truth::BoxCollider::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
		_ar& m_size;
	}
}

