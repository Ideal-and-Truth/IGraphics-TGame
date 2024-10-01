#include "UI.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Texture.h"
#include "ISprite.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::UI)

Truth::UI::UI()
	: m_sprite()
	, m_texturePath(L"../Resources/Textures/PlayerRe/T_eyes_BaseMap.png")
	, m_scale{1.0f, 1.0f}
	, m_isActive(true)
	, m_alpha(1.0f)
	, m_zDepth(0.0f)
{
}

Truth::UI::~UI()
{
	m_managers.lock()->Graphics()->DeleteSprite(m_sprite);
}

void Truth::UI::Initalize()
{
	auto gp = m_managers.lock()->Graphics();

	m_sprite = gp->CreateSprite();

	auto tex = gp->CreateTexture(m_texturePath);
	m_sprite->SetTexture(tex->m_texture);

	m_sprite->SetScale(m_scale);
	m_sprite->SetPosition(m_position);
	m_sprite->SetActive(m_isActive);
	m_sprite->SetAlpha(m_alpha);
	m_sprite->SetZ(m_zDepth);
	m_sprite->SetSampleRect({ 0, 0, tex->w, tex->h });
}

void Truth::UI::Awake()
{

}

void Truth::UI::Update()
{

}
#ifdef EDITOR_MODE
 
void Truth::UI::EditorSetValue()
{
	auto gp = m_managers.lock()->Graphics();

	auto tex = gp->CreateTexture(m_texturePath);
	m_sprite->SetTexture(tex->m_texture);

	m_sprite->SetScale(m_scale);
	m_sprite->SetPosition(m_position);
	m_sprite->SetActive(m_isActive);
	m_sprite->SetAlpha(m_alpha);
	m_sprite->SetZ(m_zDepth);
	m_sprite->SetSampleRect({ 0, 0, tex->w, tex->h });
}
#endif // IFDE
