#include "Collider.h"
#include "PhysicsManager.h"
#include "MathConverter.h"
#include "RigidBody.h"
#include "Component.h"
#include "GraphicsManager.h"
#include "RigidBody.h"

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
#ifdef _DEBUG
	, m_debugMesh(nullptr)
#endif // _DEBUG
	, m_shape()

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
#ifdef _DEBUG
	, m_debugMesh(nullptr)
#endif // _DEBUG
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
}

/// <summary>
/// Component ���� �� �۵��ϴ� �Լ�
/// </summary>
void Truth::Collider::Awake()
{
	Vector3 onwerSize = m_owner.lock()->GetScale();
	if (m_shape == ColliderShape::MESH)
	{
		m_collider = CreateCollider(m_shape, (m_size * onwerSize) / 2, m_points);
	}
	else
	{
		m_collider = CreateCollider(m_shape, (m_size * onwerSize) / 2);
	}

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

physx::PxShape* Truth::Collider::CreateCollider(ColliderShape _shape, const Vector3& _args, const std::vector<float>& _points)
{
	return m_managers.lock()->Physics()->CreateCollider(_shape, _args, _points);
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

void Truth::Collider::Initalize(const std::wstring& _path /*= L""*/)
{
#ifdef _DEBUG
	switch (m_shape)
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
		m_debugMesh = m_managers.lock()->Graphics()->CreateMesh(_path);
		break;
	}
	default:
		break;
	}

	m_managers.lock()->Graphics()->AddDebugobject(m_debugMesh);
#endif // _DEBUG

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
}
