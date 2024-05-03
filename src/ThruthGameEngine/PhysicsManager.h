#pragma once
#include "Headers.h"

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
		physx::PxDefaultAllocator m_allocator;
		physx::PxDefaultErrorCallback m_errorCallback;
		physx::PxFoundation* m_foundation;

		physx::PxPhysics* m_physics;

		physx::PxDefaultCpuDispatcher* m_dispatcher;
		physx::PxScene* m_scene;
		physx::PxMaterial* m_material;

		// �����
		physx::PxPvd* m_pvd;

	public:
		PhysicsManager();
		~PhysicsManager();

		void Initalize();
		void Finalize();

	private:
		void SetPhysics();
		void SetScene();
		void SetMaterial();
	};
}

