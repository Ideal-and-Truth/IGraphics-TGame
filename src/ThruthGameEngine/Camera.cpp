#include "Camera.h"
#include "Managers.h"
#include "ICamera.h"
#include "InputManager.h"
#include "GraphicsManager.h"

Truth::Camera::Camera()
	: Component()
{
	m_name = "Camera";
}

Truth::Camera::~Camera()
{

}

void Truth::Camera::Start()
{
	
}

void Truth::Camera::Update()
{
	float dt = GetDeltaTime();
	float speed = 300;
	m_camera->SetPosition(m_position);
	
	m_camera->SetLook(m_look);
// 	if (GetKey(KEY::W))
// 	{
// 		m_camera->Walk(dt * speed);
// 		// m_camera->SetLook(Vector3(0.0f, 1.0f, 1.0f));
// 	}
// 	if (GetKey(KEY::S))
// 	{
// 		m_camera->Walk(-dt * speed);
// 	}
// 	if (GetKey(KEY::A))
// 	{
// 		m_camera->Strafe(-dt * speed);
// 	}
// 	if (GetKey(KEY::D))
// 	{
// 		m_camera->Strafe(dt * speed);
// 	}
// 
// 	if (GetKey(KEY::LMOUSE))
// 	{
// 		m_camera->Pitch(MouseDy() * 0.003f);
// 		m_camera->RotateY(MouseDx() * 0.003f);
// 	}
}

void Truth::Camera::SetLens(float _fovY, float _aspect, float _nearZ, float _farZ)
{
	m_camera->SetLens(_fovY, _aspect, _nearZ, _farZ);
	m_fov = _fovY;
	m_aspect = _aspect;
	m_nearZ = _nearZ;
	m_farZ = _farZ;
}

void Truth::Camera::SetMainCamera()
{
	m_managers.lock()->Graphics()->SetMainCamera(m_camera);
}

void Truth::Camera::SetLook(Vector3 _val)
{
	m_camera->SetLook(_val);
}

void Truth::Camera::Pitch(float angle)
{
	m_camera->Pitch(angle);
}

void Truth::Camera::RotateY(float angle)
{
	m_camera->RotateY(angle);
}

void Truth::Camera::Awake()
{
	m_camera = m_managers.lock()->Graphics()->CreateCamera();
	//m_camera->SetPosition(Vector3(0.f, 0.f, -150.f));
	m_position = { 0.f, 0.f, -150.f };
	SetLens(0.25f * 3.141592f, m_managers.lock()->Graphics()->GetAspect(), 1.f, 100000.f);
	SetMainCamera();
}
