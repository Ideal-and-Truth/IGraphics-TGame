#pragma once
#include "Component.h"
#include "Headers.h"

namespace Truth
{
	class Transform;
}

namespace physx
{
	class PxRigidDynamic;
}

namespace Truth
{
	class RigidBody :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(RigidBody)

	public:
		float m_mass;
		float m_drag;
		float m_angularDrag;

		bool m_useGravity;
		bool m_isKinematic;

		PROPERTY(freezePosition)
		bool m_freezePosition[3];
		bool m_freezeRotation[3];

		Vector3 m_velocity;
		physx::PxRigidDynamic* m_body;

	private:
		float m_speed;
		Vector3 m_angularVelocity;
		Vector3 m_inertiaTensor;
		Vector3 m_inertiaTensorRotation;
		Vector3 m_localMassCenter;
		Vector3 m_worldMassCenter;

		std::weak_ptr<Transform> m_transform;

	public:
		RigidBody(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner);
		virtual ~RigidBody();

		void FixedUpdate(std::any _p);

		void FreezePosition(bool _xzy[3]);
		void FreezeRotation(bool _xzy[3]);

	private:
		METHOD(Awake);
		void Awake();
		
// 		METHOD(Start);
// 		void Start();
	};
}

