#include "MathConverter.h"

physx::PxVec3 MathConverter::Convert(const Vector3& _val)
{
	return physx::PxVec3(_val.x, _val.y, _val.z);
}

physx::PxExtendedVec3 MathConverter::ConvertEx(const Vector3& _val)
{
	return physx::PxExtendedVec3(_val.x, _val.y, _val.z);
}

DirectX::SimpleMath::Vector3 MathConverter::Convert(const physx::PxExtendedVec3& _val)
{
	return Vector3
	(
		static_cast<float>(_val.x),
		static_cast<float>(_val.y),
		static_cast<float>(_val.z)
	);
}

physx::PxTransform MathConverter::Convert(Matrix _val)
{
	Vector3 pos;
	Quaternion rot;
	Vector3 scale;

	_val.Decompose(scale, rot, pos);

	physx::PxTransform result;
	result.p = Convert(pos);
	result.q = Convert(rot);
	return result;
}

physx::PxQuat MathConverter::Convert(const Quaternion& _val)
{
	return physx::PxQuat(_val.x, _val.y, _val.z, _val.w);
}

DirectX::SimpleMath::Quaternion MathConverter::Convert(const physx::PxQuat& _val)
{
	return Quaternion(_val.x, _val.y, _val.z, _val.w);
}

DirectX::SimpleMath::Matrix MathConverter::Convert(const physx::PxTransform& _val)
{
	Vector3 pos = Convert(_val.p);
	Quaternion rot = Convert(_val.q);

	Matrix result = Matrix::Identity;
	result.Translation(pos);
	result.CreateFromQuaternion(rot);
	return result;
}

DirectX::SimpleMath::Vector3 MathConverter::Convert(const physx::PxVec3& _val)
{
	return Vector3(_val.x, _val.y, _val.z);
}

physx::PxVec2 MathConverter::Convert(const Vector2& _val)
{
	return physx::PxVec2(_val.x, _val.y);
}
