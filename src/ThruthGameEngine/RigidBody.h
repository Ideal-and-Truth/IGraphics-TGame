#pragma once
#include "Component.h"
#include "Headers.h"

namespace Truth
{
	class Transform;

	struct RigidBodyDecs
	{
		float m_mass;
		float m_drag;
		float m_angularDrag;

		bool m_useGravity;
		bool m_isKinematic;

		bool m_freezePosition[3];
		bool m_freezeRotation[3];

		Vector3 m_velocity;
	};
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
		PROPERTY(mass);
		float m_mass;
		PROPERTY(drag);
		float m_drag;
		PROPERTY(angularDrag);
		float m_angularDrag;

		PROPERTY(useGravity);
		bool m_useGravity;
		PROPERTY(isKinematic);
		bool m_isKinematic;

		PROPERTY(freezePosition);
		bool m_freezePosition[3];
		PROPERTY(freezeRotation);
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

		bool m_isChanged;

	public:
		RigidBody();
		virtual ~RigidBody();

		void FixedUpdate(std::any _p);

		void FreezePosition(bool _xzy[3]);
		void FreezeRotation(bool _xzy[3]);

		METHOD(Update);
		void Update() override;

		void AddImpulse(Vector3 _force);
		void SetLinearVelocity(Vector3 _val);
		Vector3 GetLinearVelocity() const;

		void InitalizeMassAndInertia();

		void SetRigidData(RigidBodyDecs _data);
	private:
		METHOD(Awake);
		void Awake();

		METHOD(Initalize);
		void Initalize();


		METHOD(Start);
		void Start();
	};
}

