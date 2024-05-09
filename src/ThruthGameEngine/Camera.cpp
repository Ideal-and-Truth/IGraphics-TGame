#include "Camera.h"
#include "Managers.h"
#include "ICamera.h"
#include "InputManager.h"
#include "GraphicsManager.h"

Truth::Camera::Camera(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner)
	: Component(_managers, _owner)
{
	m_camera = m_managers.lock()->Graphics()->CreateCamera();
	SetLens(0.25f * 3.141592f, m_managers.lock()->Graphics()->GetAspect(), 1.f, 3000.f);
	EventBind<Camera>("Update", &Camera::Update);
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
		m_camera->Walk(-dt * speed);
	}
	if (GetKey(KEY::DOWN))
	{
		m_camera->Walk(dt * speed);
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
