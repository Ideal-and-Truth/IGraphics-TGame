#include "Controller.h"
#include "PhysicsManager.h"
#include "MathConverter.h"
#include "RigidBody.h"
#include "Entity.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Controller)

Truth::Controller::Controller()
	: m_controller(nullptr)
	, m_minmumDistance(0.0001f)
	, m_flag(0)
	, m_rigidbody(nullptr)
{
	m_name = "TestController";
}

Truth::Controller::~Controller()
{

}

/// <summary>
/// �ʱ�ȭ
/// </summary>
void Truth::Controller::Initalize()
{
	m_controller = m_managers.lock()->Physics()->CreatePlayerController();
	m_rigidbody = std::make_shared<RigidBody>();

	m_rigidbody->m_transform = m_owner.lock()->GetComponent<Transform>();
	m_rigidbody->m_owner = m_owner.lock();
	m_rigidbody->m_isController = true;
	m_rigidbody->m_controller = m_controller;

	m_rigidbody->m_body = m_controller->getActor();
	m_controller->getActor()->userData = m_rigidbody.get();
}

/// <summary>
/// Controller�� Actor�� Scene�� �����
/// </summary>
void Truth::Controller::Start()
{
	m_managers.lock()->Physics()->AddScene(m_controller->getActor());
}

/// <summary>
/// ������ �Լ�
/// </summary>
/// <param name="_disp">�ӵ�</param>
void Truth::Controller::Move(Vector3& _disp)
{
	m_flag |= 
		static_cast<uint32>(
			m_controller->move
		(
			MathConverter::Convert(_disp),
			m_minmumDistance,
			1.0f / 60.0f, 
			physx::PxControllerFilters()
		));
}

/// <summary>
/// ���� ��ġ�� �̵� �Լ�
/// </summary>
/// <param name="_disp">��ġ</param>
/// <returns>���� ����</returns>
bool Truth::Controller::SetPosition(Vector3& _disp)
{
	return m_controller->setPosition(physx::PxExtendedVec3(_disp.x , _disp.y, _disp.z));
}

/// <summary>
/// ȸ�� ����
/// </summary>
void Truth::Controller::SetRotation(Quaternion& _val)
{
	m_rigidbody->SetRotation(_val);
}

/// <summary>
/// Collider�� �Ʒ��� �浹�� �ִ���
/// </summary>
/// <returns>�浹 ����</returns>
bool Truth::Controller::IsCollisionDown()
{
	return static_cast<physx::PxControllerCollisionFlag::Enum>(m_flag) & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
}
/// <summary>
/// Collider�� ���� �浹�� �ִ���
/// </summary>
/// <returns>�浹 ����</returns>
bool Truth::Controller::IsCollisionUp()
{
	return static_cast<physx::PxControllerCollisionFlag::Enum>(m_flag) & physx::PxControllerCollisionFlag::eCOLLISION_UP;
}
/// <summary>
/// Collider�� ���� �浹�� �ִ���
/// </summary>
/// <returns>�浹 ����</returns>
bool Truth::Controller::IsCollisionSide()
{
	return static_cast<physx::PxControllerCollisionFlag::Enum>(m_flag) & physx::PxControllerCollisionFlag::eCOLLISION_SIDES;
}
