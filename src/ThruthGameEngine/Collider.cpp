#include "Collider.h"
#include "PhysicsManager.h"
#include "MathConverter.h"
#include "RigidBody.h"
#include "Component.h"
#include "GraphicsManager.h"
#include "RigidBody.h"
#include "Controller.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Collider)

/// <summary>
/// �ݶ��̴� ������
/// </summary>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::Collider::Collider(bool _isTrigger /*= true*/)
	: Component()
	, m_isTrigger(_isTrigger)
	, m_center{ 0.0f, 0.0f, 0.0f }
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
	, m_rigidbody()
#ifdef EDITOR_MODE
	, m_debugMesh(nullptr)
#endif // EDITOR_MODE
	, m_shape()
	, m_enable(true)
	, m_isController(false)

{
	m_center = { 0.0f, 0.0f, 0.0f };
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// �ݶ��̴� ������
/// </summary>
/// <param name="_pos">�߽���</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::Collider::Collider(Vector3 _pos, bool _isTrigger /*= true*/)
	: m_center(_pos)
	, m_isTrigger(_isTrigger)
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
	, m_rigidbody()
	, m_shape()
	, m_enable(true)
	, m_isController(false)
#ifdef EDITOR_MODE
	, m_debugMesh(nullptr)
#endif // EDITOR_MODE
{
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// �Ҹ���
/// �ݶ��̴��� physx scene�� ��� �ִٸ� ��ȯ�Ѵ�.
/// </summary>
Truth::Collider::~Collider()
{
	if (m_collider != nullptr)
	{
		m_collider->release();
		m_collider->userData = nullptr;
		m_collider = nullptr;
	}

#ifdef EDITOR_MODE
	if (m_debugMesh != nullptr)
	{
		m_managers.lock()->Graphics()->DeleteDebugMeshObject(m_debugMesh);
		m_debugMesh = nullptr;
	}
#endif // EDITOR_MODE
}

/// <summary>
/// ����� �Ҹ���
/// Entity�� game scene���� ���� �� �� ����ȴ�.
/// </summary>
void Truth::Collider::Destroy()
{
	if (m_collider)
	{
		m_collider->userData = nullptr;
		m_body->detachShape(*m_collider);
	}

#ifdef EDITOR_MODE
	// 	if (m_debugMesh != nullptr)
	// 	{
	// 		m_managers.lock()->Graphics()->DeleteMeshObject(m_debugMesh);
	// 		m_debugMesh = nullptr;
	// 	}
#endif // EDITOR_MODE
}

/// <summary>
/// Component ���� �� �۵��ϴ� �Լ�
/// </summary>
void Truth::Collider::Awake()
{
	if (m_shape == ColliderShape::MESH)
	{
		return;
	}

	Vector3 finalSize = {};
	Vector3 finalPos = {};
	Quaternion temp = {};



	bool isSRT = (m_owner.lock()->GetWorldTM() * m_localTM).Decompose(finalSize, temp, finalPos);

	if (!isSRT)
	{
		MathConverter::DecomposeNonSRT(finalSize, temp, finalPos, (m_owner.lock()->GetWorldTM() * m_localTM));
	}

	Vector3 onwerSize = m_owner.lock()->GetWorldScale();
	m_collider = CreateCollider(m_shape, (finalSize) / 2);
	m_collider->userData = this;

	m_collider->setLocalPose(physx::PxTransform(
		MathConverter::Convert(m_center)
	));

	SetUpFiltering(m_owner.lock()->m_layer);

	m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
	m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);

	m_rigidbody = m_owner.lock()->GetComponent<RigidBody>();
	auto con = m_owner.lock()->GetComponent<Controller>();

	if (!con.expired())
	{
		m_rigidbody = con.lock()->GetRigidbody();
		m_body = m_rigidbody.lock()->m_body;
		m_body->attachShape(*m_collider);

		auto af = m_body->getActorFlags();
		auto rf = m_body->getBaseFlags();
		auto sf = m_collider->getFlags();
		return;
	}

	if (m_rigidbody.expired())
	{
		m_body = m_managers.lock()->Physics()->CreateDefaultRigidStatic();
		m_body->attachShape(*m_collider);
		physx::PxTransform t(
			MathConverter::Convert(m_owner.lock()->GetWorldPosition()),
			MathConverter::Convert(m_owner.lock()->GetWorldRotation())
		);
		m_body->setGlobalPose(t);
		m_managers.lock()->Physics()->AddScene(m_body);
		return;
	}

	m_body = m_rigidbody.lock()->m_body;
}

void Truth::Collider::FixedUpdate()
{
	physx::PxTransform t(
		MathConverter::Convert(m_owner.lock()->GetWorldPosition()),
		MathConverter::Convert(m_owner.lock()->GetWorldRotation())
	);
	m_body->setGlobalPose(t);
}

/// <summary>
/// Collider�� �߽��� ���Ѵ�
/// </summary>
/// <param name="_pos">�߽���</param>
void Truth::Collider::SetCenter(Vector3 _pos)
{
	m_center = _pos;
	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

/// <summary>
/// Collider�� ũ�⸦ ���Ѵ� (���ũ��)
/// </summary>
/// <param name="_size">ũ��</param>
void Truth::Collider::SetSize(Vector3 _size)
{
	m_size = _size;
	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

/// <summary>
/// �浹 ó���� ���� �ʴ´�
/// </summary>
void Truth::Collider::OnDisable()
{
	if (m_enable == true)
	{
		return;
	}
	m_enable = false;
	m_body->detachShape(*m_collider);
}

/// <summary>
/// �浹 ó���� �簳�Ѵ�
/// </summary>
void Truth::Collider::OnEnable()
{
	if (m_enable == false)
	{
		return;
	}
	m_enable = true;
	m_body->attachShape(*m_collider);
}

#ifdef EDITOR_MODE
/// <summary>
/// ����� �Ž��� �׸��� ���� �Լ�
/// </summary>
void Truth::Collider::ApplyTransform()
{
	const Matrix& ownerTM = m_owner.lock()->GetWorldTM();
	m_globalTM = m_localTM * ownerTM;
	m_debugMesh->SetTransformMatrix(m_globalTM);

}

/// <summary>
/// �����Ϳ��� ������ ���� �����Ѵ� 
/// </summary>
void Truth::Collider::EditorSetValue()
{
	SetCenter(m_center);
	SetSize(m_size);

	if (m_enable)
	{
		OnDisable();
	}
	else
	{
		OnEnable();
	}
}
#endif // EDITOR_MODE

/// <summary>
/// �ݶ��̴��� physx�� ���� �����Ѵ�
/// ������ �� �� phsyx scene�� ���ε� �Ǵ°��� �ƴϴ�.
/// </summary>
/// <param name="_shape">��� (ť��, ��, ĸ��)</param>
/// <param name="_args">�� ��翡 �´� �Ķ���� (���� ����, ����, ������ ����)</param>
/// <returns></returns>
physx::PxShape* Truth::Collider::CreateCollider(ColliderShape _shape, const Vector3& _args)
{
	return m_managers.lock()->Physics()->CreateCollider(_shape, _args);
}

/// <summary>
/// ����Ʈ ���̳��� �ٵ� ����
/// Rigidbody�� ���� �ݶ��̴��� ��� �ش� �ٵ� ���
/// </summary>
/// <returns></returns>
physx::PxRigidDynamic* Truth::Collider::GetDefaultDynamic()
{
	return m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
}

/// <summary>
/// ����Ʈ ����ƽ �ٵ� ����
/// Rigidbody�� ���� �ݶ��̴��� ��� �ش� �ٵ� ���
/// </summary>
/// <returns></returns>
physx::PxRigidStatic* Truth::Collider::GetDefaultStatic()
{
	return m_managers.lock()->Physics()->CreateDefaultRigidStatic();
}

/// <summary>
/// �ʱ�ȭ
/// </summary>
/// <param name="_path">����� �Ž����</param>
void Truth::Collider::Initalize(const std::wstring& _path /*= L""*/)
{
#ifdef EDITOR_MODE
	switch (m_shape)
	{
	case Truth::ColliderShape::BOX:
	{
		m_debugMesh = m_managers.lock()->Graphics()->CreateDebugMeshObject(L"DebugObject/debugCube");
		break;
	}
	case Truth::ColliderShape::SPHERE:
	{
		m_debugMesh = m_managers.lock()->Graphics()->CreateDebugMeshObject(L"DebugObject/debugSphere");
		break;
	}
	case Truth::ColliderShape::CAPSULE:
	{
		break;
	}
	case Truth::ColliderShape::MESH:
	{
		m_debugMesh = m_managers.lock()->Graphics()->CreateDebugMeshObject(_path);
		break;
	}
	default:
		break;
	}

#endif // EDITOR_MODE

	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

/// <summary>
/// ���� ����
/// �浹 ó���� ���� ���� ���� ��Ÿ���� ���� ������ ����
/// </summary>
/// <param name="_filterGroup"></param>
void Truth::Collider::SetUpFiltering(uint32 _filterGroup)
{
	physx::PxFilterData filterData;
	filterData.word0 = _filterGroup;
	m_collider->setSimulationFilterData(filterData);
	m_collider->setQueryFilterData(filterData);
}


