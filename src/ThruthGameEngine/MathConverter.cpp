#include "MathConverter.h"

physx::PxVec3 MathConverter::Convert(const Vector3& _val)
{
	return physx::PxVec3(_val.x, _val.y, _val.z);
}

bool MathConverter::DecomposeNonSRT(Vector3& _outScale, Quaternion& _outRot, Vector3& _outPos, const Matrix& _mat)
{
	float def = _mat.Determinant();
	if (abs(def) <= 0.00001f)
	{
		return false;
	}
	DirectX::XMMATRIX mat = DirectX::XMMATRIX(_mat);

	_outPos = mat.r[3];

	_outScale.x = DirectX::XMVectorGetX(DirectX::XMVector3Length(mat.r[0])); // X축 스케일
	_outScale.y = DirectX::XMVectorGetY(DirectX::XMVector3Length(mat.r[1])); // Y축 스케일
	_outScale.z = DirectX::XMVectorGetZ(DirectX::XMVector3Length(mat.r[2])); // Z축 스케일

	DirectX::XMMATRIX rotationMatrix;
	rotationMatrix.r[0] = Vector3(mat.r[0]) / _outScale.x;
	rotationMatrix.r[1] = Vector3(mat.r[1]) / _outScale.y;
	rotationMatrix.r[2] = Vector3(mat.r[2]) / _outScale.z;
	rotationMatrix.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);  // 나머지 행렬 성분

	rotationMatrix.r[0] = DirectX::XMVector3Normalize(rotationMatrix.r[0]);
	rotationMatrix.r[1] = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rotationMatrix.r[1], Vector3(DirectX::XMVector3Dot(rotationMatrix.r[0], rotationMatrix.r[1])) * Vector3(rotationMatrix.r[0])));
	rotationMatrix.r[2] = DirectX::XMVector3Cross(rotationMatrix.r[0], rotationMatrix.r[1]);

	_outRot = XMQuaternionRotationMatrix(rotationMatrix);

	return true;
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
