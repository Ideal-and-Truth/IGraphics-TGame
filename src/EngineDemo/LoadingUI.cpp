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
	m_sprite->SetPosition({ 0, 0 });
	m_sprite->SetScale({ 1920.f, 1080.f });


	EventBind("Loading", &LoadingUI::Active);
}

void LoadingUI::Update()
{
	if (!m_isActive)
		return;

	float speed = m_speed * GetDeltaTime();

	m_alpha += speed;

	m_sprite->SetAlpha(m_alpha);
	if (m_alpha >= 2.0f)
	{
		EventPublish("SaveData", nullptr);
		m_managers.lock()->Scene()->ChangeScene(m_nextMap);
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
