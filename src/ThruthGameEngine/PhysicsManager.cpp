#include "PhysicsManager.h"

Truth::PhysicsManager::PhysicsManager()
	: m_allocator{}
	, m_errorCallback{}
	, m_foundation(nullptr)
	, m_physics(nullptr)
	, m_dispatcher(nullptr)
	, m_scene(nullptr)
	, m_material(nullptr)
	, m_pvd(nullptr)
{
}

Truth::PhysicsManager::~PhysicsManager()
{
}

void Truth::PhysicsManager::Initalize()
{
	SetPhysics();
	SetScene();
	SetMaterial();
}

void Truth::PhysicsManager::Finalize()
{

}

void Truth::PhysicsManager::SetPhysics()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);
	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true);
}

void Truth::PhysicsManager::SetScene()
{
	physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	m_scene = m_physics->createScene(sceneDesc);
}

void Truth::PhysicsManager::SetMaterial()
{
	m_material = m_physics->createMaterial(0.5f, 0.5f, 0.0f);
}
