#include "MeshCollider.h"
#include "FileUtils.h"
#include "PhysicsManager.h"
#include "MathConverter.h"
#include "RigidBody.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::MeshCollider)

Truth::MeshCollider::MeshCollider()
	: Collider(false)
{
	m_size = Vector3{ 1.0f, 1.0f, 1.0f };
	m_path = L"TestMap/navTestMap";
	m_shape = ColliderShape::MESH;
}

Truth::MeshCollider::~MeshCollider()
{

}

void Truth::MeshCollider::Initalize()
{
	Collider::Initalize(m_path);
}

void Truth::MeshCollider::Awake()
{
	Vector3 onwerSize = m_owner.lock()->GetScale();

	GetPoints();
	if (m_points.empty())
	{
		return;
	}
	m_meshCollider = m_managers.lock()->Physics()->CreateMeshCollider((m_size * onwerSize), m_points);
	m_isTrigger = false;

	// SetUpFiltering(m_owner.lock()->m_layer);
	for (auto* mc : m_meshCollider)
	{
		mc->userData = this;

		mc->setLocalPose(physx::PxTransform(
			MathConverter::Convert(m_center)
		));


		mc->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		mc->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	}


	m_rigidbody = m_owner.lock()->GetComponent<RigidBody>();

	if (m_rigidbody.expired())
	{
		m_body = m_managers.lock()->Physics()->CreateDefaultRigidStatic();
		for (auto* mc : m_meshCollider)
		{
			m_body->attachShape(*mc);
		}
		physx::PxTransform t(
			MathConverter::Convert(m_owner.lock()->GetPosition()),
			MathConverter::Convert(m_owner.lock()->GetRotation())
		);
		m_body->setGlobalPose(t);
		m_managers.lock()->Physics()->AddScene(m_body);
	}
}

/// <summary>
/// 경로에서 정점 데이터를 가져온다
/// </summary>
void Truth::MeshCollider::GetPoints()
{
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	std::wstring prefix = L"../Resources/Models/";
	file->Open(prefix + m_path + L".pos", FileMode::Read);

	unsigned int meshNum = file->Read<unsigned int>();
	for (unsigned int i = 0; i < meshNum; i++)
	{
		m_points.push_back(std::vector<Vector3>());
		unsigned int verticesNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < verticesNum; j++)
		{
			Vector3 p;
			p.x = file->Read<float>();
			p.y = file->Read<float>();
			p.z = file->Read<float>();
			m_points[i].push_back(p);
		}

		unsigned int temp = file->Read<unsigned int>();
		for (unsigned int j = 0; j < temp; j++)
		{
			unsigned int t = file->Read<uint32>();
		}
	}
}