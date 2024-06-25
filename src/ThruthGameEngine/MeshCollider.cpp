#include "MeshCollider.h"
BOOST_CLASS_EXPORT_IMPLEMENT(Truth::MeshCollider)

Truth::MeshCollider::MeshCollider()
	: Collider()
{
	m_size = Vector3{ 1.0f, 1.0f, 1.0f };
	_path = L"PhysxSampleObject/SampleStair";
}

Truth::MeshCollider::~MeshCollider()
{

}

void Truth::MeshCollider::Initalize()
{
	Collider::Initalize(_path);
}
