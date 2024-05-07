#pragma once
#include "Headers.h"

// 라이브러리에 경고가 너무 많다
// 나중에 이 부분 수정 예정
// 뭔가 다른 방법이 있겠지
#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 33010)
#pragma warning(disable: 6297)
#include "physx/PxPhysicsAPI.h"
#pragma warning(pop)

/// <summary>
/// PhysX 라이브러리를 사용하는 매니저
/// 모든 PhysX 기능은 이 라이브러리를 통해 실행 될 것이다.
/// </summary>
namespace Truth
{
	class PhysicsManager
	{
	private:
		// Foundation 생성
		physx::PxDefaultAllocator m_allocator;
		physx::PxDefaultErrorCallback m_errorCallback;
		physx::PxFoundation* m_foundation;

		physx::PxPhysics* m_physics;

		physx::PxDefaultCpuDispatcher* m_dispatcher;
		physx::PxScene* m_scene;
		physx::PxMaterial* m_material;

		// 디버거
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

