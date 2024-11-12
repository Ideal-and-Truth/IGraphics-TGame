#include "LoadingUI.h"
#include "GraphicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(LoadingUI)

LoadingUI::LoadingUI()
{
}

LoadingUI::~LoadingUI()
{
}

void LoadingUI::Start()
{
	fs::path p = m_path;
	m_sprite = m_managers.lock()->Graphics()->CreateSprite();
	auto tex = m_managers.lock()->Graphics()->CreateTexture(p, false, false, false);
	m_sprite->SetTexture(tex->m_texture);
	m_sprite->SetAlpha(m_alpha);
	if (m_path == "../Resources/Textures/UI/Loading/Loading.png")
	{
		m_sprite->SetPosition({ 1550.f, 900.f });
		m_sprite->SetScale({ 1.f, 1.f });
	}
	else
	{
		m_sprite->SetPosition({ 0, 0 });
		m_sprite->SetScale({ 1920.f, 1080.f });
	}


	EventBind("Loading", &LoadingUI::Active);
}

void LoadingUI::Update()
{
	if (GetKeyDown(KEY::F10) && m_nextMap == m_cheat)
	{
		m_managers.lock()->Scene()->ChangeScene(m_nextMap);
	}
	if (!m_isActive)
		return;

	float speed = m_speed * GetDeltaTime();

	m_alpha += speed;

	m_sprite->SetAlpha(m_alpha);
	if (m_alpha >= 2.0f)
	{
		EventPublish("SaveData", nullptr);
		if (m_nextMap != m_cheat)
		{
			m_managers.lock()->Scene()->ChangeScene(m_nextMap);
		}
	}
}

void LoadingUI::Destroy()
{
	m_managers.lock()->Graphics()->DeleteSprite(m_sprite);
}

void LoadingUI::Active(const void* _)
{
	m_isActive = true;
}
