#include "MathConverter.h"

physx::PxVec3 MathConverter::Convert(Vector3 _val)
{
	return physx::PxVec3(_val.x, _val.y, _val.z);
}

DirectX::SimpleMath::Vector3 MathConverter::Convert(physx::PxVec3 _val)
{
	return Vector3(_val.x, _val.y, _val.z);
}

physx::PxVec2 MathConverter::Convert(Vector2 _val)
{
	return physx::PxVec2(_val.x, _val.y);
}
