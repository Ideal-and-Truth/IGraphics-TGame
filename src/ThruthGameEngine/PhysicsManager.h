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
#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#pragma warning(pop)

#ifdef _DEBUG
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/LowLevel_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/LowLevelAABB_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/LowLevelDynamics_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysX_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXCommon_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXCooking_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXExtensions_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXFoundation_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXTask_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXVehicle_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/SceneQuery_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/SimulationController_static_64.lib")
#else
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/LowLevel_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/LowLevelAABB_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/LowLevelDynamics_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysX_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXCommon_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXCooking_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXExtensions_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXFoundation_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXTask_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXVehicle_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/SceneQuery_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/SimulationController_static_64.lib")
#endif // DEBUG

namespace Truth
{
	class PxEventCallback;
}

namespace physx
{
	static uint8 m_collsionTable[8];
};

/// <summary>
/// PhysX ���̺귯���� ����ϴ� �Ŵ���
/// ��� PhysX ����� �� ���̺귯���� ���� ���� �� ���̴�.
/// </summary>
namespace Truth
{
	class PhysicsManager
	{
	private:
		Truth::PxEventCallback* collisionCallback;

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

		void SetCollisionFilter(uint8 _layerA, uint8 _layerB, bool _isCollisoin);

	private:
		void CreateStack(const physx::PxTransform& _t, physx::PxU32 _size, physx::PxReal _halfExtent);
		physx::PxRigidDynamic* createDynamic(const physx::PxTransform& _t, const physx::PxGeometry& _geometry, const physx::PxVec3& _velocity = physx::PxVec3(0));
	};

	physx::PxFilterFlags FilterShaderExample(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
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

