#include "Camera.h"
#include "Managers.h"
#include "ICamera.h"
#include "InputManager.h"
#include "GraphicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Camera)

Truth::Camera::Camera()
	: Component()
	, m_fov(0.25f * 3.141592f)
	, m_aspect(1)
	, m_nearZ(1)
	, m_farZ(1000)
{
	m_name = "Camera";
}

Truth::Camera::~Camera()
{

}

void Truth::Camera::Update()
{
	float dt = GetDeltaTime();
	float speed = 300;
	if (GetKey(KEY::W))
	{
		m_camera->Walk(dt * speed);
	}
	if (GetKey(KEY::S))
	{
		m_camera->Walk(-dt * speed);
	}
	if (GetKey(KEY::A))
	{
		m_camera->Strafe(-dt * speed);
	}
	if (GetKey(KEY::D))
	{
		m_camera->Strafe(dt * speed);
	}

	if (GetKey(KEY::LMOUSE))
	{
		m_camera->Pitch(MouseDy() * 0.003f);
		m_camera->RotateY(MouseDx() * 0.003f);
	}
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

void Truth::Camera::Awake()
{

}

void Truth::Camera::Start()
{
	SetMainCamera();
}

void Truth::Camera::Initalize()
{
	m_camera = m_managers.lock()->Graphics()->CreateCamera();
	m_camera->SetPosition(Vector3(0.f, 0.f, -150.f));
	SetLens(0.25f * 3.141592f, m_managers.lock()->Graphics()->GetAspect(), 1.f, 100000.f);
}

#ifdef _DEBUG
void Truth::Camera::EditorSetValue()
{
	m_camera->SetLens(m_fov, m_aspect, m_nearZ, m_farZ);
}
#endif // _DEBUG
