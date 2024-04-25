#pragma once

namespace Ideal
{
	class ICamera
	{
	public:
		ICamera() {};
		virtual ~ICamera() {};

	public:
		virtual void SetLens(float FovY, float Aspect, float NearZ, float FarZ) abstract;

		virtual void Walk(float Distance) abstract;
		virtual void Strafe(float Distance) abstract;
		virtual void Pitch(float Angle) abstract;
		virtual void RotateY(float Angle) abstract;
	};
}