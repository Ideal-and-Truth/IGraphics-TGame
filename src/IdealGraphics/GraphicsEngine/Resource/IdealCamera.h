#pragma once
#include "GraphicsEngine/public/ICamera.h"
#include "Core/Core.h"

namespace Ideal
{
	class IdealCamera : public ICamera
	{
	public:
		IdealCamera();
		virtual ~IdealCamera();

	public:
		virtual void SetLens(float FovY, float Aspect, float NearZ, float FarZ) override;

		virtual void Walk(float d) override;
		virtual void Strafe(float d) override;
		virtual void Pitch(float Angle) override;
		virtual void RotateY(float Angle) override;
		
		virtual void SetPosition(const Vector3& Position) override;
		virtual void SetLook(Vector3 Look) override;

		DirectX::SimpleMath::Matrix GetView()const { return m_view; }
		DirectX::SimpleMath::Matrix GetProj()const { return m_proj; }
		DirectX::SimpleMath::Matrix GetViewProj()const { return m_view * m_proj; }
		void UpdateViewMatrix();

		// ver2
		void UpdateMatrix2();

		Vector3 GetPosition() { return m_position; }
	private:
		DirectX::SimpleMath::Vector3 m_position;
		DirectX::SimpleMath::Vector3 m_right;
		DirectX::SimpleMath::Vector3 m_up;
		DirectX::SimpleMath::Vector3 m_look;

		float m_nearZ;
		float m_farZ;
		float m_aspect;
		float m_fovY;
		float m_nearWindowHeight;
		float m_farWindowHeight;

		bool m_viewDirty;

		DirectX::SimpleMath::Matrix m_view;
		DirectX::SimpleMath::Matrix m_proj;
	};
}