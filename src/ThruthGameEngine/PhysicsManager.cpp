#include "PhysicsManager.h"
#include "PxEventCallback.h"
#include "Collider.h"
#include "MathConverter.h"
#include "RigidBody.h"
#include "FileUtils.h"

// static uint8 physx::m_collsionTable[8] = {};

/// <summary>
/// 생성자
/// </summary>
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
	, m_cooking(nullptr)
{
	for (uint8 i = 0; i < 8; i++)
	{
		for (uint8 j = 0; j < 8; j++)
		{
			physx::m_collsionTable[i] += 1 << j;
		}
	}
}

/// <summary>
/// 소멸자
/// </summary>
Truth::PhysicsManager::~PhysicsManager()
{
	Finalize();
}

/// <summary>
/// 초기화
/// physx 데이터를 초기화 하고 새 Scene을 생성한다.
/// </summary>
void Truth::PhysicsManager::Initalize()
{
	// physx 의 기본 요소를 생성하는 foundation
	m_foundation = ::PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

	// visual debugger 생성 
#ifdef _DEBUG
	m_pvd = physx::PxCreatePvd(*m_foundation);
	m_trasport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pvd->connect(*m_trasport, physx::PxPvdInstrumentationFlag::eALL);
#endif // _DEBUG

	// physx에서 실제 물리 연산을 할 객체를 생성하는 physics 클래스
	m_physics = ::PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);
#ifdef _DEBUG
	::PxInitExtensions(*m_physics, m_pvd);
#endif // _DEBUG

	// 이벤트 콜백 함수
	collisionCallback = new Truth::PxEventCallback();

	assert(m_physics && "PxCreatePhysics failed!");

	// Cpu 코어 사용 갯수
	m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);

	// 씬 생성
	CreatePhysxScene();

	m_cooking = ::PxCreateCooking(
		PX_PHYSICS_VERSION,
		*m_foundation,
		physx::PxCookingParams(physx::PxTolerancesScale())
	);
}

/// <summary>
/// 소멸 시 physx 데이터 초기화
/// </summary>
void Truth::PhysicsManager::Finalize()
{
	// 	m_physics->release();
	// 	m_pvd->release();
	// 	m_trasport->release();
}

/// <summary>
/// 업데이트 (필요시 사용)
/// </summary>
void Truth::PhysicsManager::Update()
{
}

/// <summary>
/// 고정 업데이트
/// 60프레임 속도로 업데이트
/// </summary>
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
					pos = MathConverter::Convert(rigidbody->GetController()->getFootPosition());
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

				rigidbody->SetLocalTM(rigidbody->m_invertLocalTM * TM);
			}
		}
	}
}

/// <summary>
/// physx reset
/// Scene을 재생성하고, 필요시 PVD와 재 연결한다.
/// </summary>
void Truth::PhysicsManager::ResetPhysX()
{
	m_scene->release();

	CreatePhysxScene();

#ifdef _DEBUG
	m_pvd->disconnect();
	m_pvd->connect(*m_trasport, physx::PxPvdInstrumentationFlag::eALL);
#endif // _DEBUG
}

/// <summary>
/// Scene에 Actor를 추가한다
/// </summary>
/// <param name="_actor">추가할 엑터</param>
void Truth::PhysicsManager::AddScene(physx::PxActor* _actor)
{
	m_scene->addActor(*_actor);
}

/// <summary>
/// 동적 바디 생성
/// </summary>
/// <param name="_pos">위치</param>
/// <param name="_rot">회전</param>
/// <returns>동적 바디</returns>
physx::PxRigidDynamic* Truth::PhysicsManager::CreateRigidDynamic(Vector3 _pos, Quaternion _rot)
{
	auto body = m_physics->createRigidDynamic(
		physx::PxTransform(
			physx::PxVec3(_pos.x, _pos.y, _pos.z)
		)
	);
	return body;
}

/// <summary>
/// 고정 바디 생성
/// </summary>
/// <param name="_pos">위치</param>
/// <param name="_rot">회전</param>
/// <returns>동적 바디</returns>
physx::PxRigidStatic* Truth::PhysicsManager::CreateRigidStatic(Vector3 _pos, Quaternion _rot)
{
	auto body = m_physics->createRigidStatic(
		physx::PxTransform(
			physx::PxVec3(_pos.x, _pos.y, _pos.z)
		)
	);
	return body;
}

/// <summary>
/// 원점에 회전하지 않은 동적 바디 생성
/// </summary>
/// <returns>동적 바디</returns>
physx::PxRigidDynamic* Truth::PhysicsManager::CreateDefaultRigidDynamic()
{
	physx::PxRigidDynamic* body = CreateRigidDynamic(Vector3::Zero, Quaternion::Identity);
	// 	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	// 	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	// 	body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	return body;
}
/// <summary>
/// 원점에 회전하지 않은 정적 바디 생성
/// </summary>
/// <returns>정적 바디</returns>
physx::PxRigidStatic* Truth::PhysicsManager::CreateDefaultRigidStatic()
{
	physx::PxRigidStatic* body = CreateRigidStatic(Vector3::Zero, Quaternion::Identity);
	return body;
}

/// <summary>
/// 콜라이더 생성
/// </summary>
/// <param name="_shape">모양</param>
/// <param name="_args">파라미터</param>
/// <param name="_points">점 정보</param>
/// <returns>콜라이다</returns>
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
	default:
		shape = nullptr;
		break;
	}

	return shape;
}

std::vector<physx::PxShape*> Truth::PhysicsManager::CreateMeshCollider(const Vector3& _args, const std::vector<std::vector<Vector3>>& _points /*= std::vector<std::vector<Vector3>>()*/)
{
	if (_points.empty())
	{
		return std::vector<physx::PxShape*>();
	}
	std::vector<physx::PxShape*> shape;

	shape.resize(_points.size());

	std::vector<std::vector<physx::PxVec3>> convexVerts;
	convexVerts = ConvertPointToVertex(_args, _points);

	for (int i = 0; i < _points.size(); i++)
	{
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = static_cast<physx::PxU32>(convexVerts[i].size());
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = convexVerts[i].data();
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;

		if (!m_cooking->cookConvexMesh(convexDesc, buf, &result))
			return std::vector<physx::PxShape*>();

		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());

		physx::PxConvexMesh* convexMesh = m_physics->createConvexMesh(input);

		shape[i] = m_physics->createShape(physx::PxConvexMeshGeometry(convexMesh), *m_material);
	}
	return shape;
}

/// <summary>
/// 필터 세팅
/// </summary>
/// <param name="_layerA">A 레이어</param>
/// <param name="_layerB">B 레이어</param>
/// <param name="_isCollisoin">설정할 충돌 여부</param>
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

/// <summary>
/// 컨트롤러 생성
/// </summary>
/// <param name="_desc">생성 구조체</param>
/// <returns>컨트롤러</returns>
physx::PxController* Truth::PhysicsManager::CreatePlayerController(const physx::PxCapsuleControllerDesc& _desc)
{
	physx::PxController* c = m_CCTManager->createController(_desc);

	assert(c && "cannot create Controller\n");

	return c;
}

/// <summary>
/// 물리 머테리얼 생성
/// </summary>
/// <param name="_val">매질 정보 (정적 마찰, 동적 마찰, 탄성)</param>
/// <returns>물리 머테리얼</returns>
physx::PxMaterial* Truth::PhysicsManager::CreateMaterial(Vector3 _val)
{
	return m_physics->createMaterial(_val.x, _val.y, _val.z);
}

/// <summary>
/// 특정 파일에 있는 내용대로 맵의 콜라이더를 만든다.
/// </summary>
/// <param name="_path"></param>
void Truth::PhysicsManager::CreateMapCollider(const std::wstring& _path)
{
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(_path, FileMode::Read);

	std::vector<std::vector<physx::PxVec3>> vers;
	std::vector<std::vector<uint32>> inds;


	uint32 meshCount = file->Read<uint32>();
	vers.resize(meshCount);
	inds.resize(meshCount);

	for (size_t i = 0; i < meshCount; i++)
	{
		uint32 verCount = file->Read<uint32>();
		vers[i].resize(verCount);
		for (size_t j = 0; j < verCount; j++)
		{
			vers[i][j].x = file->Read<float>();
			vers[i][j].y = file->Read<float>();
			vers[i][j].z = file->Read<float>();
		}

		uint32 indCount = file->Read<uint32>();
		inds[i].resize(indCount);
		for (size_t j = 0; j < indCount; j++)
		{
			inds[i][j] = file->Read<uint32>();
		}
	}

	for (auto& m : vers)
	{
		CreateMapCollider(m);
	}
}

/// <summary>
/// physx의 Scene을 생성하는 함수
/// </summary>
void Truth::PhysicsManager::CreatePhysxScene()
{
	// 씬 구조체
	physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	// 기본적으로 적용되는 중력
	sceneDesc.gravity = physx::PxVec3(0.0f, -100.0f, 0.0f);
	sceneDesc.cpuDispatcher = m_dispatcher;
	// 씬에 사용될 필터
	sceneDesc.filterShader = FilterShaderExample;
	sceneDesc.simulationEventCallback = collisionCallback;

	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;

	// 씬 생성
	m_scene = m_physics->createScene(sceneDesc);

#ifdef _DEBUG
	// 출력 할 디버깅 정보
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 4.0f);
	physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
#endif // _DEBUG

	auto s = m_scene->getBounceThresholdVelocity();
	m_scene->setBounceThresholdVelocity(100.0f);

#ifdef _DEBUG
	// 디버거 플래그
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
#endif // _DEBUG

	// 기본 바닥 만들기 (필요없는 경우 없애면 된다)
	m_material = m_physics->createMaterial(0.5f, 0.5f, 0.5f);
	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*m_physics, physx::PxPlane(0, 1, 0, 0), *m_material);
	m_scene->addActor(*groundPlane);

	// 컨트롤러 매니저 만들기
	m_CCTManager = PxCreateControllerManager(*m_scene);
}

/// <summary>
/// 점 데이터를 Convex Mesh 사용할 정점 데이터로 변환 
/// </summary>
/// <param name="_points">점 데이터</param>
/// <returns>정점 데이터</returns>
std::vector<std::vector<physx::PxVec3>> Truth::PhysicsManager::ConvertPointToVertex(const Vector3& _args, const std::vector<std::vector<Vector3>>& _points)
{
	std::vector<std::vector<physx::PxVec3>> result;
	result.resize(_points.size());

	for (int i = 0; i < _points.size(); i++)
	{
		result[i].resize(_points[i].size());
		for (int j = 0; j < _points[i].size(); j++)
		{
			result[i][j].x = _points[i][j].x * _args.x;
			result[i][j].y = _points[i][j].y * _args.y;
			result[i][j].z = _points[i][j].z * _args.z;
		}
	}

	return result;
}

void Truth::PhysicsManager::CreateMapCollider(const std::vector<physx::PxVec3>& _vers)
{
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = static_cast<physx::PxU32>(_vers.size());
	convexDesc.points.stride = sizeof(physx::PxVec3);
	convexDesc.points.data = _vers.data();
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;

	if (!m_cooking->cookConvexMesh(convexDesc, buf, &result))
	{
		return;
	}

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());

	physx::PxConvexMesh* convexMesh = m_physics->createConvexMesh(input);

	physx::PxShape* shape = m_physics->createShape(physx::PxConvexMeshGeometry(convexMesh), *m_material);
	shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);

	physx::PxRigidStatic* body = CreateDefaultRigidStatic();

	body->attachShape(*shape);

	body->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	body->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);

	m_scene->addActor(*body);
}

/// <summary>
/// 필터 셰이더
/// </summary>
/// <param name="attributes0">0번 오브젝트의 트리거 여부</param>
/// <param name="filterData0">0번 오브젝트의 필터 정보</param>
/// <param name="attributes1">1번 오브젝트의 트리거 여부</param>
/// <param name="filterData1">1번 오브젝트의 필터 정보</param>
/// <param name="pairFlags">충돌 플래그</param>
/// <param name="constantBlock">unused</param>
/// <param name="constantBlockSize">unused</param>
/// <returns>콜백 함수에 반환 할 플래그</returns>
physx::PxFilterFlags Truth::FilterShaderExample(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
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
