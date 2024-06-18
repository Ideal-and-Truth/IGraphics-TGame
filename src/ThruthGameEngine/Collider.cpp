#include "Collider.h"
#include "PhysicsManager.h"
#include "MathConverter.h"
#include "RigidBody.h"
#include "Component.h"
#include "GraphicsManager.h"
#include "RigidBody.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Collider)

/// <summary>
/// 콜라이더 생성자
/// </summary>
/// <param name="_isTrigger">트리거 여부</param>
Truth::Collider::Collider(bool _isTrigger /*= true*/)
	: Component()
	, m_isTrigger(_isTrigger)
	, m_center{ 0.0f, 0.0f, 0.0f }
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
	, m_rigidbody()
#ifdef _DEBUG
	, m_debugMesh(nullptr)
#endif // _DEBUG
	, m_shape()

{
	m_center = { 0.0f, 0.0f, 0.0f };
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// 콜라이더 생성자
/// </summary>
/// <param name="_pos">중심점</param>
/// <param name="_isTrigger">트리거 여부</param>
Truth::Collider::Collider(Vector3 _pos, bool _isTrigger /*= true*/)
	: m_center(_pos)
	, m_isTrigger(_isTrigger)
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
	, m_rigidbody()
#ifdef _DEBUG
	, m_debugMesh(nullptr)
#endif // _DEBUG
{
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// 소멸자
/// 콜라이더가 physx scene에 살아 있다면 반환한다.
/// </summary>
Truth::Collider::~Collider()
{
	if (m_collider != nullptr)
	{
		m_collider->release();
		m_collider->userData = nullptr;
		m_collider = nullptr;
	}
}

/// <summary>
/// 명시적 소멸자
/// Entity가 game scene에서 삭제 될 때 실행된다.
/// </summary>
void Truth::Collider::Destroy()
{
	if (m_collider)
	{
		m_collider->userData = nullptr;
		m_body->detachShape(*m_collider);
	}
}

/// <summary>
/// Component 생성 시 작동하는 함수
/// </summary>
void Truth::Collider::Awake()
{


	Vector3 onwerSize = m_owner.lock()->GetScale();
	m_collider = CreateCollider(m_shape, (m_size * onwerSize) / 2);

	m_collider->userData = this;

	m_collider->setLocalPose(physx::PxTransform(
		MathConverter::Convert(m_center)
	));

	SetUpFiltering(m_owner.lock()->m_layer);

	m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
	m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);

	m_rigidbody = m_owner.lock()->GetComponent<RigidBody>();

	if (m_rigidbody.expired())
	{
		m_body = m_managers.lock()->Physics()->CreateDefaultRigidStatic();
		m_body->attachShape(*m_collider);
		physx::PxTransform t(
			MathConverter::Convert(m_owner.lock()->GetPosition()),
			MathConverter::Convert(m_owner.lock()->GetRotation())
		);
		m_body->setGlobalPose(t);
		m_managers.lock()->Physics()->AddScene(m_body);
	}
}

void Truth::Collider::Start()
{

}

void Truth::Collider::SetCenter(Vector3 _pos)
{
	m_center = _pos;
	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

void Truth::Collider::SetSize(Vector3 _size)
{
	m_size = _size;
	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

#ifdef _DEBUG
void Truth::Collider::ApplyTransform()
{
	const Matrix& ownerTM = m_owner.lock()->GetWorldTM();
	m_globalTM = m_localTM * ownerTM;
	m_debugMesh->SetTransformMatrix(m_globalTM);

}

void Truth::Collider::EditorSetValue()
{
	SetCenter(m_center);
	SetSize(m_size);
}

#endif // _DEBUG

/// <summary>
/// 콜라이더를 physx를 통해 생성한다
/// 생성만 할 뿐 phsyx scene에 업로드 되는것은 아니다.
/// </summary>
/// <param name="_shape">모양 (큐브, 구, 캡슐)</param>
/// <param name="_args">각 모양에 맞는 파라미터 (변의 길의, 지름, 지름과 높이)</param>
/// <returns></returns>
physx::PxShape* Truth::Collider::CreateCollider(ColliderShape _shape, const Vector3& _args)
{
	return m_managers.lock()->Physics()->CreateCollider(_shape, _args);
}

/// <summary>
/// 디폴트 다이나믹 바디 생성
/// Rigidbody가 없는 콜라이더의 경우 해당 바디 사용
/// </summary>
/// <returns></returns>
physx::PxRigidDynamic* Truth::Collider::GetDefaultDynamic()
{
	return m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
}

/// <summary>
/// 디폴트 스태틱 바디 생성
/// Rigidbody가 없는 콜라이더의 경우 해당 바디 사용
/// </summary>
/// <returns></returns>
physx::PxRigidStatic* Truth::Collider::GetDefaultStatic()
{
	return m_managers.lock()->Physics()->CreateDefaultRigidStatic();
}

void Truth::Collider::Initalize(ColliderShape _shape, const Vector3& _param)
{
	m_shape = _shape;
#ifdef _DEBUG
	switch (_shape)
	{
	case Truth::ColliderShape::BOX:
	{
		m_debugMesh = m_managers.lock()->Graphics()->CreateMesh(L"DebugObject/debugCube");
		break;
	}
	case Truth::ColliderShape::SPHERE:
	{
		m_debugMesh = m_managers.lock()->Graphics()->CreateMesh(L"DebugObject/debugSphere");
		break;
	}
	case Truth::ColliderShape::CAPSULE:
	{
		break;
	}
	case Truth::ColliderShape::MESH:
	{
		break;
	}
	default:
		break;
	}

	m_managers.lock()->Graphics()->AddDebugobject(m_debugMesh);

	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);

#endif // _DEBUG
}

/// <summary>
/// 필터 세팅
/// 충돌 처리에 예외 사항 등을 나타내는 필터 데이터 세팅
/// </summary>
/// <param name="_filterGroup"></param>
void Truth::Collider::SetUpFiltering(uint32 _filterGroup)
{
	physx::PxFilterData filterData;
	filterData.word0 = _filterGroup;
	m_collider->setSimulationFilterData(filterData);
}
