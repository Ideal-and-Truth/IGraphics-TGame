#include "Camera.h"
#include "Managers.h"
#include "ICamera.h"
#include "InputManager.h"
#include "GraphicsManager.h"

Truth::Camera::Camera()
	: Component()
{

}

Truth::Camera::~Camera()
{

}

void Truth::Camera::Update(std::any _p)
{
	float dt = GetDeltaTime();
	float speed = 100;
	if (GetKey(KEY::UP))
	{
		// m_camera->Walk(dt * speed);
		m_camera->SetLook(Vector3(0.0f, 1.0f, 1.0f));
	}
	if (GetKey(KEY::DOWN))
	{
		m_camera->Walk(-dt * speed);
	}
	if (GetKey(KEY::LEFT))
	{
		m_camera->Strafe(-dt * speed);
	}
	if (GetKey(KEY::RIGHT))
	{
		m_camera->Strafe(dt * speed);
	}
}

void Truth::Camera::SetLens(float _fovY, float _aspect, float _nearZ, float _farZ)
{
	m_camera->SetLens(_fovY, _aspect, _nearZ, _farZ);
}

void Truth::Camera::SetMainCamera()
{
	m_managers.lock()->Graphics()->SetMainCamera(m_camera);
}

void Truth::Camera::Awake()
{
	m_camera = m_managers.lock()->Graphics()->CreateCamera();
	m_camera->SetPosition(Vector3(0.f, 0.f, -150.f));
	SetLens(0.25f * 3.141592f, m_managers.lock()->Graphics()->GetAspect(), 1.f, 3000.f);
	SetMainCamera();

	EventBind<Camera>("Update", &Camera::Update);
}
