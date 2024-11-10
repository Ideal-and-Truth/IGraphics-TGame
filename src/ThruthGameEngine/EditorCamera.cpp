#include "EditorCamera.h"
#ifdef EDITOR_MODE
#include "Managers.h"
#include "GraphicsManager.h"
#include "TimeManager.h"

Truth::EditorCamera::EditorCamera(Managers* _managers)
	: m_camera(nullptr)
	, m_speed(50.0f)
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
	float speed = m_speed;

	if (GetKey(KEY::LSHIFT))
	{
		speed = m_speed * 0.1f;
	}

	if (GetKey(KEY::W))
	{
		m_camera->Walk(_dt * speed);
	}
	if (GetKey(KEY::S))
	{
		m_camera->Walk(-_dt * speed);
	}
	if (GetKey(KEY::A))
	{
		m_camera->Strafe(-_dt * speed);
	}
	if (GetKey(KEY::D))
	{
		m_camera->Strafe(_dt * speed);
	}

	if (GetKey(KEY::E))
	{
		m_camera->SetPosition(m_camera->GetPosition() + Vector3(0.0f, _dt * speed, 0.0f));
	}
	if (GetKey(KEY::Q))
	{
		m_camera->SetPosition(m_camera->GetPosition() + Vector3(0.0f, -_dt * speed, 0.0f));
	}

	if (GetKey(MOUSE::RMOUSE))
	{
		m_camera->Pitch(MouseDy() * 0.003f);
		m_camera->RotateY(MouseDx() * 0.003f);
	}

	static float timer = 0.0f;
	timer += m_managers->Time()->GetDT();
	while (timer >= 1.0f)
	{
		timer -= 1.0f;
		Vector3 pos = m_camera->GetPosition();
		Vector3 look = m_camera->GetLook();
		std::string sPos = "Pos: " + std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z) + "\n";
		std::string slook = "Look: " + std::to_string(look.x) + " " + std::to_string(look.y) + " " + std::to_string(look.z) + "\n";
		DEBUG_PRINT(sPos.c_str());
		DEBUG_PRINT(slook.c_str());
	}
}

bool Truth::EditorCamera::GetKey(KEY _key)
{
	return m_managers->Input()->GetKeyState(_key) == KEY_STATE::HOLD;
}

bool Truth::EditorCamera::GetKey(MOUSE _key)
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
#endif // EDITOR_MODE
