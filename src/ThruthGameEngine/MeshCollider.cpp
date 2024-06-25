#include "MeshCollider.h"
BOOST_CLASS_EXPORT_IMPLEMENT(Truth::MeshCollider)

Truth::MeshCollider::MeshCollider()
	: Collider(false)
{
	m_size = Vector3{ 0.1f, 0.1f, 0.1f };
	m_path = L"PhysxSampleObject/SampleStair";
	m_shape = ColliderShape::MESH;
}

Truth::MeshCollider::~MeshCollider()
{

}

void Truth::MeshCollider::Initalize()
{
	Collider::Initalize(m_path);
}
