#include "EditorCamera.h"
#include "Managers.h"
#include "GraphicsManager.h"

Truth::EditorCamera::EditorCamera(Managers* _managers)
	: m_camera(nullptr)
	, m_speed(300.0f)
	, m_managers(_managers)
{
	m_camera = m_managers->Graphics()->CreateCamera();
	m_camera->SetPosition(Vector3(0.f, 0.f, -150.f));
	m_camera->SetLensWithoutAspect(0.25f * 3.141592f, 1.f, 100000.f);
	SetMainCamera();
}

Truth::EditorCamera::~EditorCamera()
{
}

void Truth::EditorCamera::Update(float _dt)
{
	if (GetKey(KEY::W))
	{
		m_camera->Walk(_dt * m_speed);
	}
	if (GetKey(KEY::S))
	{
		m_camera->Walk(-_dt * m_speed);
	}
	if (GetKey(KEY::A))
	{
		m_camera->Strafe(-_dt * m_speed);
	}
	if (GetKey(KEY::D))
	{
		m_camera->Strafe(_dt * m_speed);
	}

	if (GetKey(KEY::LMOUSE))
	{
		m_camera->Pitch(MouseDy() * 0.003f);
		m_camera->RotateY(MouseDx() * 0.003f);
	}
}

bool Truth::EditorCamera::GetKey(KEY _key)
{
	return m_managers->Input()->GetKeyState(_key) == KEY_STATE::HOLD;
}

float Truth::EditorCamera::MouseDy()
{
	return m_managers->Input()->GetMouseMoveY();
}

float Truth::EditorCamera::MouseDx()
{
	return m_managers->Input()->GetMouseMoveX();
}

void Truth::EditorCamera::SetMainCamera()
{
	m_managers->Graphics()->SetMainCamera(this);
}
