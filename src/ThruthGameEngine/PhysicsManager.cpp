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
	, m_trasport(nullptr)
{
}

Truth::PhysicsManager::~PhysicsManager()
{
}

void Truth::PhysicsManager::Initalize()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

	// m_oPvd = PxCreateOmniPvd(*m_foundation);

	m_pvd = physx::PxCreatePvd(*m_foundation);
	m_trasport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	bool c = m_pvd->connect(*m_trasport, physx::PxPvdInstrumentationFlag::eALL);

	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);

	static Truth::TruthPxEventCallback collisionCallback;

	assert(m_physics && "PxCreatePhysics failed!");

	physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	m_scene = m_physics->createScene(sceneDesc);
	bool cflag = false;
	cflag = m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	// cflag = m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	cflag = m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 4.0f);
	m_scene->setSimulationEventCallback(&collisionCallback);
	physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();

	if (pvdClient)
	{
 		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
 		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
 		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*m_physics, physx::PxPlane(0, 1, 0, 0), *m_material);
	m_scene->addActor(*groundPlane);

	for (physx::PxU32 i = 0; i < 5; i++)
	{
		CreateStack(physx::PxTransform(physx::PxVec3(0, 0, m_stackZ -= 10.0f)), 10, 2.0f);
	}

	if (!m_isInteractive)
	{
		createDynamic(physx::PxTransform(physx::PxVec3(0, 40, 100)), physx::PxSphereGeometry(10), physx::PxVec3(0, -50, -100));
	}
}

void Truth::PhysicsManager::Finalize()
{
	m_physics->release();
	m_pvd->release();
	m_trasport->release();
}

void Truth::PhysicsManager::Update()
{
	m_scene->simulate(0.02f);
	physx::PxU32 a;
	bool c = m_scene->fetchResults(true, &a);

}

void Truth::PhysicsManager::ResetPhysX()
{

}

void Truth::PhysicsManager::CreateStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent)
{
	physx::PxShape* shape = m_physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_material);
	for (physx::PxU32 i = 0; i < size; i++)
	{
		for (physx::PxU32 j = 0; j < size - i; j++)
		{
			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
			physx::PxRigidDynamic* body = m_physics->createRigidDynamic(t.transform(localTm));
			body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
			body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
			body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
			body->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			m_scene->addActor(*body);
		}
	}
	shape->release();
}

physx::PxRigidDynamic* Truth::PhysicsManager::createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity /*= physx::PxVec3(0)*/)
{
	physx::PxRigidDynamic* dynamic = PxCreateDynamic(*m_physics, t, geometry, *m_material, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	m_scene->addActor(*dynamic);
	return dynamic;
}
