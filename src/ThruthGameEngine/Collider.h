#pragma once
#include "Component.h"
#include "PhysicsManager.h"

namespace Truth
{
	class Collider :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Collider)

	public:
		bool m_isTrigger;
		Vector3 m_center;

	protected:
		physx::PxShape* m_collider;
		physx::PxActor* m_body;

	public:
		Collider(std::shared_ptr<Managers> _managers, bool _isTrigger = true);
		virtual ~Collider();

	protected:
		inline physx::PxShape* CreateCollider(ColliderShape _shape, const std::vector<float>& _args)
		{
			return m_managers.lock()->Physics()->CreateCollider(_shape, _args);
		}

		inline physx::PxActor* GetDefaultDynamic()
		{
			return m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
		}
		inline physx::PxActor* GetDefaultStatic()
		{
			return m_managers.lock()->Physics()->CreateDefaultRigidStatic();
		}
	};
}

