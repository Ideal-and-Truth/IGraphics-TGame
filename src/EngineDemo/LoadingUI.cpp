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
}

void LoadingUI::Update()
{
}
