#pragma once
#include "Headers.h"
#include "ShapeType.h"

// ���̺귯���� ��� �ʹ� ����
// ���߿� �� �κ� ���� ����
// ���� �ٸ� ����� �ְ���
#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 33010)
#pragma warning(disable: 6297)
#include "physx/PxPhysicsAPI.h"
#pragma warning(pop)

/// <summary>
/// PhysX ���̺귯���� ����ϴ� �Ŵ���
/// ��� PhysX ����� �� ���̺귯���� ���� ���� �� ���̴�.
/// </summary>
namespace Truth
{
	class PhysicsManager
	{
	private:
		// Foundation ����
		physx::PxFoundation* m_foundation;
		physx::PxDefaultAllocator m_allocator;
		physx::PxDefaultErrorCallback m_errorCallback;
	
		physx::PxPhysics* m_physics;

		physx::PxDefaultCpuDispatcher* m_dispatcher;
		physx::PxScene* m_scene;
		physx::PxMaterial* m_material;

		// �����
		physx::PxPvd* m_pvd;
		physx::PxPvdTransport* m_trasport;

		physx::PxReal m_stackZ = 20.0f;

		// physx::PxOmniPvd* m_oPvd;

		bool m_isInteractive = false;

	public:
		PhysicsManager();
		~PhysicsManager();

		void Initalize();
		void Finalize();
		void Update();
		void FixedUpdate();

		void ResetPhysX();

		void AddScene(physx::PxActor* _actor);

		physx::PxMaterial* GetMaterial() { return m_material; };

		physx::PxRigidDynamic* CreateRigidDynamic(Vector3 _pos, Quaternion _rot);
		physx::PxRigidStatic* CreateRigidStatic(Vector3 _pos, Quaternion _rot);

		physx::PxRigidDynamic* CreateDefaultRigidDynamic();
		physx::PxRigidStatic* CreateDefaultRigidStatic();

		physx::PxShape* CreateCollider(ColliderShape _shape, const std::vector<float>& _args);

	private:
		void CreateStack(const physx::PxTransform& _t, physx::PxU32 _size, physx::PxReal _halfExtent);
		physx::PxRigidDynamic* createDynamic(const physx::PxTransform& _t, const physx::PxGeometry& _geometry, const physx::PxVec3& _velocity = physx::PxVec3(0));
	};
}

namespace Truth
{
	class TruthPxEventCallback
		: public physx::PxSimulationEventCallback
	{
	public:
		virtual void onContact(const physx::PxContactPairHeader& _pairHeader, const physx::PxContactPair* _pairs, physx::PxU32 _nbPairs) {};
		virtual void onTrigger(physx::PxTriggerPair* _pairs, physx::PxU32 _count) {};
		virtual void onWake(physx::PxActor** _actors, physx::PxU32 _count) {};
		virtual void onSleep(physx::PxActor** _actors, physx::PxU32 _count) {};
		virtual void onAdvance(const physx::PxRigidBody* const* _bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 _count) {};
		virtual void onConstraintBreak(physx::PxConstraintInfo* _constraints, physx::PxU32 _count) {};
	};
}