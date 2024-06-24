#pragma once
#include "Component.h"
#include "ShapeType.h"

// #include "PhysicsManager.h"

namespace physx
{
	class PxShape;
	class PxRigidDynamic;
	class PxRigidActor;
	class PxRigidStatic;
	struct PxFilterData;
}

namespace Truth
{
	class RigidBody;
}



namespace Truth
{
	class Collider abstract 
		: public Component
	{
		GENERATE_CLASS_TYPE_INFO(Collider);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

		ColliderShape m_shape;

	public:
		bool m_isTrigger;

		PROPERTY(center);
		Vector3 m_center;
		PROPERTY(size);
		Vector3 m_size;

		Matrix m_localTM;
		Matrix m_globalTM;

		inline static uint32 m_colliderIDGenerator = 0;
		uint32 m_colliderID;
		physx::PxShape* m_collider;

		physx::PxRigidActor* m_body;
		std::weak_ptr<RigidBody> m_rigidbody;

#ifdef _DEBUG
		std::shared_ptr<Ideal::IMeshObject> m_debugMesh;
#endif // _DEBUG

	protected:

	public:
		Collider(bool _isTrigger = true);
		Collider(Vector3 _pos, bool _isTrigger = true);
		virtual ~Collider();

		METHOD(Destroy);
		void Destroy();

		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();

		METHOD(SetCenter);
		void SetCenter(Vector3 _pos);

		METHOD(SetSize);
		void SetSize(Vector3 _size);

#ifdef _DEBUG
		METHOD(ApplyTransform);
		void ApplyTransform();

		void EditorSetValue();
#endif // _DEBUG

	protected:
		physx::PxShape* CreateCollider(ColliderShape _shape, const Vector3& _args);
		physx::PxRigidDynamic* GetDefaultDynamic();
		physx::PxRigidStatic* GetDefaultStatic();

		void Initalize(ColliderShape _shape, const Vector3& _param);

		void SetUpFiltering(uint32 _filterGroup);
	};

	template<class Archive>
	void Truth::Collider::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_isTrigger;
		_ar& m_center;
		_ar& m_size;
	}

	template<class Archive>
	void Truth::Collider::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_isTrigger;
		_ar& m_center;
		_ar& m_size;
	}

	struct Collision
	{
		std::weak_ptr<Collider> m_collA;
		std::weak_ptr<Collider> m_collB;
	};
}
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Truth::Collider)
BOOST_CLASS_EXPORT_KEY(Truth::Collider)
BOOST_CLASS_VERSION(Truth::Collider, 0)