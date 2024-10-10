#include "UI.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Texture.h"
#include "ISprite.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::UI)

Truth::UI::UI()
	: m_sprite()
	, m_texturePath()
	, m_scale{ 1.0f, 1.0f }
	, m_isActive(true)
	, m_alpha(1.0f)
	, m_zDepth(0.0f)
{

}

Truth::UI::~UI()
{
	for (uint32 i = 0; i < 3; i++)
	{
		if (m_sprite[i] != nullptr)
		{
			m_managers.lock()->Graphics()->DeleteSprite(m_sprite[i]);
		}
	}
}

void Truth::UI::Initalize()
{
	auto gp = m_managers.lock()->Graphics();
	for (uint32 i = 0; i < 3; i++)
	{
		if (!m_texturePath[i].empty())
		{
			m_sprite[i] = gp->CreateSprite();
			auto tex = gp->CreateTexture(m_texturePath[i]);
			m_texture[i] = tex;
			m_sprite[i]->SetTexture(tex->m_texture);
			m_sprite[i]->SetScale(m_scale);
			m_sprite[i]->SetPosition(m_position);
			m_sprite[i]->SetActive(m_isActive);
			m_sprite[i]->SetAlpha(m_alpha);
			m_sprite[i]->SetZ(m_zDepth);
			m_sprite[i]->SetSampleRect({ 0, 0, tex->w, tex->h });
		}
	}
}

void Truth::UI::Update()
{

}

void Truth::UI::OnMouseOver()
{
	m_state = BUTTON_STATE::OVER;
}

void Truth::UI::OnMouseClick()
{
	m_state = BUTTON_STATE::DOWN;
}

void Truth::UI::OnMouseUp()
{
	m_state = BUTTON_STATE::IDEL;
}

#ifdef EDITOR_MODE
void Truth::UI::EditorSetValue()
{
	auto gp = m_managers.lock()->Graphics();
	for (uint32 i = 0; i < 3; i++)
	{
		m_sprite[i]->SetScale(m_scale);
		m_sprite[i]->SetPosition(m_position);
		m_sprite[i]->SetActive(m_isActive);
		m_sprite[i]->SetAlpha(m_alpha);
		m_sprite[i]->SetZ(m_zDepth);
	}
}
#endif // IFDE
