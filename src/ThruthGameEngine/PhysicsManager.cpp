#include "PhysicsManager.h"
#include "PxEventCallback.h"
#include "Collider.h"
#include "MathConverter.h"
#include "RigidBody.h"

// static uint8 physx::m_collsionTable[8] = {};

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
	, collisionCallback(nullptr)
	, m_CCTManager(nullptr)
{
	for (uint8 i = 0; i < 8; i++)
	{
		for (uint8 j = 0; j < 8; j++)
		{
			physx::m_collsionTable[i] += 1 << j;
		}
	}

	// SetCollisionFilter(1, 2, false);
}

Truth::PhysicsManager::~PhysicsManager()
{
	DEBUG_PRINT("Finalize PhysicsManager\n");
}

void Truth::PhysicsManager::Initalize()
{
	m_foundation = ::PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

	// m_oPvd = PxCreateOmniPvd(*m_foundation);

	m_pvd = physx::PxCreatePvd(*m_foundation);
	m_trasport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pvd->connect(*m_trasport, physx::PxPvdInstrumentationFlag::eALL);

	m_physics = ::PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);
	::PxInitExtensions(*m_physics, m_pvd);
	collisionCallback = new Truth::PxEventCallback();

	assert(m_physics && "PxCreatePhysics failed!");

	m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);

	CreatePhysxScene();
}

void Truth::PhysicsManager::Finalize()
{
	m_physics->release();
	m_pvd->release();
	m_trasport->release();
}

void Truth::PhysicsManager::Update()
{
}

void Truth::PhysicsManager::FixedUpdate()
{


	m_scene->simulate(1.0f / 60.0f);
	m_scene->fetchResults(true);

	physx::PxU32 nbActiveActors;
	physx::PxActor** activeActors = m_scene->getActiveActors(nbActiveActors);

	for (physx::PxU32 i = 0; i < nbActiveActors; ++i)
	{
		physx::PxRigidActor* rigidActor = activeActors[i]->is<physx::PxRigidActor>();
		if (rigidActor)
		{
			RigidBody* rigidbody = static_cast<RigidBody*>(rigidActor->userData);
			if (rigidbody)
			{
				Vector3 pos;
				Quaternion rot;
				Vector3 scale;

				if (rigidbody->IsController())
				{
					auto p = rigidbody->GetController()->getPosition();
					pos = MathConverter::Convert(rigidbody->GetController()->getPosition());
					rot = rigidbody->GetRotation();
				}
				else
				{
					 physx::PxTransform physxTM = rigidActor->getGlobalPose();
					pos = MathConverter::Convert(physxTM.p);
					rot = MathConverter::Convert(physxTM.q);
				}
				scale = rigidbody->GetScale();

				Matrix TM = Matrix::CreateScale(scale);
				TM *= Matrix::CreateFromQuaternion(rot);
				TM *= Matrix::CreateTranslation(pos);

				rigidbody->SetWorldTM(rigidbody->m_invertLocalTM * TM);
			}
		}
	}
}

void Truth::PhysicsManager::ResetPhysX()
{
	m_scene->release();

	CreatePhysxScene();

	m_pvd->disconnect();
	m_pvd->connect(*m_trasport, physx::PxPvdInstrumentationFlag::eALL);
}

void Truth::PhysicsManager::AddScene(physx::PxActor* _actor)
{
	m_scene->addActor(*_actor);
}

physx::PxRigidDynamic* Truth::PhysicsManager::CreateRigidDynamic(Vector3 _pos, Quaternion _rot)
{
	auto body = m_physics->createRigidDynamic(
		physx::PxTransform(
			physx::PxVec3(_pos.x, _pos.y, _pos.z)
		)
	);
	return body;
}

physx::PxRigidStatic* Truth::PhysicsManager::CreateRigidStatic(Vector3 _pos, Quaternion _rot)
{
	auto body = m_physics->createRigidStatic(
		physx::PxTransform(
			physx::PxVec3(_pos.x, _pos.y, _pos.z)
		)
	);
	return body;
}

physx::PxRigidDynamic* Truth::PhysicsManager::CreateDefaultRigidDynamic()
{
	physx::PxRigidDynamic* body = CreateRigidDynamic(Vector3::Zero, Quaternion::Identity);
	// 	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	// 	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	// 	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	return body;
}

physx::PxRigidStatic* Truth::PhysicsManager::CreateDefaultRigidStatic()
{
	physx::PxRigidStatic* body = CreateRigidStatic(Vector3::Zero, Quaternion::Identity);
	return body;
}

physx::PxShape* Truth::PhysicsManager::CreateCollider(ColliderShape _shape, const Vector3& _args)
{
	physx::PxShape* shape = nullptr;
	switch (_shape)
	{
	case Truth::ColliderShape::BOX:
	{
		shape = m_physics->createShape(physx::PxBoxGeometry(_args.x, _args.y, _args.z), *m_material);
		break;
	}
	case Truth::ColliderShape::CAPSULE:
	{
		shape = m_physics->createShape(physx::PxCapsuleGeometry(_args.x, _args.y), *m_material);
		break;
	}
	case Truth::ColliderShape::SPHERE:
	{
		shape = m_physics->createShape(physx::PxSphereGeometry(_args.x), *m_material);
		break;
	}
	case Truth::ColliderShape::MESH:
	{
		// shape = m_physics->createShape(physx::PxConvexMeshGeometry(_args[0]), *m_material);
		break;
	}
	default:
		shape = nullptr;
		break;
	}

	return shape;
}

void Truth::PhysicsManager::SetCollisionFilter(uint8 _layerA, uint8 _layerB, bool _isCollisoin)
{
	if (_isCollisoin)
	{
		physx::m_collsionTable[_layerA] |= 1 << _layerB;
		physx::m_collsionTable[_layerB] |= 1 << _layerA;
	}
	else
	{
		physx::m_collsionTable[_layerA] &= ~(1 << _layerB);
		physx::m_collsionTable[_layerB] &= ~(1 << _layerA);
	}
}

physx::PxController* Truth::PhysicsManager::CreatePlayerController()
{
	physx::PxCapsuleControllerDesc desc;

	desc.height = 1.8f;
	desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
	desc.contactOffset = 0.05f;
	desc.stepOffset = 0.1f;
	desc.radius = 0.8f;
	desc.position = physx::PxExtendedVec3(0.0f, 10.0f, 0.0f);
	desc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
	desc.material = m_physics->createMaterial(1.0f, 1.0f, 0.05f);

	physx::PxController* c = m_CCTManager->createController(desc);

	return c;
}

void Truth::PhysicsManager::CreatePhysxScene()
{
	physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -98.1f, 0.0f);
	sceneDesc.cpuDispatcher = m_dispatcher;
	sceneDesc.filterShader = FilterShaderExample;
	sceneDesc.simulationEventCallback = collisionCallback;

	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;

	m_scene = m_physics->createScene(sceneDesc);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 4.0f);
	auto s = m_scene->getBounceThresholdVelocity();
	m_scene->setBounceThresholdVelocity(100.0f);
	physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();

	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	m_material = m_physics->createMaterial(0.5f, 0.5f, 0.5f);

	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*m_physics, physx::PxPlane(0, 1, 0, 0), *m_material);
	m_scene->addActor(*groundPlane);

	m_CCTManager = PxCreateControllerManager(*m_scene);
}

void Truth::PhysicsManager::CreateStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent)
{
	physx::PxShape* shape = m_physics->createShape(physx::PxBoxGeometry(halfExtent * 10, halfExtent * 10, halfExtent), *m_material);
	// 	shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
	// 	shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);

	for (physx::PxU32 i = 0; i < size; i++)
	{
		for (physx::PxU32 j = 0; j < size - i; j++)
		{
		}
	}
	int i = 1;
	int j = 1;
	physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1) + 10, 0) * halfExtent);
	physx::PxRigidDynamic* body = m_physics->createRigidDynamic(t.transform(localTm));
	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	body->attachShape(*shape);
	// body->setLinearVelocity(physx::PxVec3(0, -50, -100));
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

	m_scene->addActor(*body);
	// body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
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

physx::PxFilterFlags Truth::FilterShaderExample(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	// 한마디로 걍 필터 없는거처럼 행동한다 이거

	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	}

	else if (physx::m_collsionTable[filterData0.word0] & (1 << filterData1.word0))
	{
		pairFlags = physx::PxPairFlag::eSOLVE_CONTACT
			| physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
			| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
			| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
			| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	}
	else
	{
		return physx::PxFilterFlag::eSUPPRESS;
	}

	return physx::PxFilterFlag::eDEFAULT;
}
