#include "MathConverter.h"

physx::PxVec3 MathConverter::Convert(Vector3 _val)
{
	return physx::PxVec3(_val.x, _val.y, _val.z);
}

physx::PxQuat MathConverter::Convert(Quaternion _val)
{
	return physx::PxQuat(_val.x, _val.y, _val.z, _val.w);
}

DirectX::SimpleMath::Quaternion MathConverter::Convert(physx::PxQuat& _val)
{
	return Quaternion(_val.x, _val.y, _val.z, _val.w);
}

DirectX::SimpleMath::Matrix MathConverter::Convert(physx::PxTransform&& _val)
{
	Vector3 pos = Convert(_val.p);
	Quaternion rot = Convert(_val.q);

	Matrix result = Matrix::Identity;
	result.Translation(pos);
	result.CreateFromQuaternion(rot);
	return result;
}

DirectX::SimpleMath::Vector3 MathConverter::Convert(physx::PxVec3 _val)
{
	return Vector3(_val.x, _val.y, _val.z);
}

physx::PxVec2 MathConverter::Convert(Vector2 _val)
{
	return physx::PxVec2(_val.x, _val.y);
}
