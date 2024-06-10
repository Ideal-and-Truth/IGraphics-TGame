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
	class Collider :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Collider);
	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	public:
		bool m_isTrigger;
		Vector3 m_center;
		Quaternion m_rotation;
		inline static uint32 m_colliderIDGenerator = 0;
		uint32 m_colliderID;
		physx::PxShape* m_collider;

	protected:
		physx::PxRigidActor* m_body;

	public:
		Collider(bool _isTrigger = true);
		Collider(Vector3 _pos, bool _isTrigger = true);
		virtual ~Collider();

		void SetPhysxTransform(Vector3 _pos, Quaternion _rot);
		void SetPhysxTransform(Vector3 _pos);

	protected:
		physx::PxShape* CreateCollider(ColliderShape _shape, const std::vector<float>& _args);
		physx::PxRigidDynamic* GetDefaultDynamic();
		physx::PxRigidStatic* GetDefaultStatic();


		void SetUpFiltering(uint32 _filterGroup);
	};

	template<class Archive>
	void Truth::Collider::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_isTrigger;
		_ar& m_center;
		_ar& m_rotation;
	}

	struct Collision
	{
		std::weak_ptr<Collider> m_collA;
		std::weak_ptr<Collider> m_collB;
	};
}
