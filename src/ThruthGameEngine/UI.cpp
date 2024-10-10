#include "UI.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Texture.h"
#include "ISprite.h"
#include "InputManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::UI)

Truth::UI::UI()
	: m_sprite()
	, m_texturePath()
	, m_size{ 100.0f, 100.0f }
	, m_alpha(1.0f)
	, m_zDepth(0.0f)
	, m_position(100.0f, 100.0f)
	, m_prevState(BUTTON_STATE::IDEL)
	, m_state(BUTTON_STATE::IDEL)
	, m_rect{}
{
	m_texturePath[0] = "../Resources/Textures/UI/title_button_basic.png";
	m_texturePath[1] = "../Resources/Textures/UI/title_button_blue.png";
	m_texturePath[2] = "../Resources/Textures/UI/title_button_click.png";
}

Truth::UI::~UI()
{
	for (uint32 i = 0; i < 3; i++)
	{
		if (m_sprite[i] != nullptr)
		{
			m_managers.lock()->Graphics()->DeleteSprite(m_sprite[i]);
			m_sprite[i] = nullptr;
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
			auto tex = gp->CreateTexture(fs::path(m_texturePath[i]));
			float w = static_cast<float>(tex->w);
			float h = static_cast<float>(tex->h);

			Vector2 gpScale = { m_size.x / w, m_size.y / h };
			Vector2 gpPosition = { m_position.x - (m_size.x * 0.5f), m_position.y - (m_size.y * 0.5f) };

			m_texture[i] = tex;
			m_sprite[i]->SetTexture(tex->m_texture);
			m_sprite[i]->SetScale(gpScale);
			m_sprite[i]->SetPosition(gpPosition);
			m_sprite[i]->SetActive(IsActive());
			m_sprite[i]->SetAlpha(m_alpha);
			m_sprite[i]->SetZ(m_zDepth);
			m_sprite[i]->SetSampleRect({ 0, 0, tex->w, tex->h });
		}
	}


	m_rect.left = static_cast<LONG>(m_position.x - (m_size.x * 0.5f));
	m_rect.top = static_cast<LONG>(m_position.y - (m_size.y * 0.5f));
	m_rect.right = static_cast<LONG>(m_position.x + (m_size.x * 0.5f));
	m_rect.bottom = static_cast<LONG>(m_position.y + (m_size.y * 0.5f));


}

void Truth::UI::Update()
{
#ifdef EDITOR_MODE
	auto gp = m_managers.lock()->Graphics();

	RECT winRect = gp->GetWindowRect();
	Vector2 realLT = gp->GetContentPosMin();
	Vector2 realRB = gp->GetContentPosMax();

	float contentW = realRB.x - realLT.x;
	float contentH = realRB.y - realLT.y;

	float winW = static_cast<float>(winRect.right - winRect.left);
	float winH = static_cast<float>(winRect.bottom - winRect.top);

	float ratioW = contentW / winW;
	float ratioH = contentH / winH;

	Vector2 editorSize = {};
	editorSize.x = m_size.x * ratioW;
	editorSize.y = m_size.y * ratioH;

	Vector2 editorPos = {};
	editorPos.x = realLT.x + (m_position.x * ratioW);
	editorPos.y = realLT.y + (m_position.y * ratioH);

	m_rect.left = static_cast<LONG>(editorPos.x - (editorSize.x * 0.5f));
	m_rect.top = static_cast<LONG>(editorPos.y - (editorSize.y * 0.5f));
	m_rect.right = static_cast<LONG>(editorPos.x + (editorSize.x * 0.5f));
	m_rect.bottom = static_cast<LONG>(editorPos.y + (editorSize.y * 0.5f));
#endif // EDITOR_MODE

	CheckState();
	switch (m_state)
	{
	case BUTTON_STATE::IDEL:
	{
		SetSpriteActive(BUTTON_STATE::IDEL);
		break;
	}
	case BUTTON_STATE::OVER:
	{
		SetSpriteActive(BUTTON_STATE::OVER);
		break;
	}
	case BUTTON_STATE::DOWN:
	{
		SetSpriteActive(BUTTON_STATE::DOWN);
		break;
	}
	case BUTTON_STATE::UP:
	{
		SetSpriteActive(BUTTON_STATE::IDEL);
		break;
	}
	default:
		break;
	}
}

void Truth::UI::CheckState()
{
	int mouseX = m_managers.lock()->Input()->m_nowMousePosX;
	int mouseY = m_managers.lock()->Input()->m_nowMousePosY;

	if (IsActive())
	{
		if (((mouseX >= m_rect.left) && (mouseX <= m_rect.right)) &&
			((mouseY >= m_rect.top) && (mouseY <= m_rect.bottom)))
		{
			if (GetKey(MOUSE::LMOUSE))
				m_state = BUTTON_STATE::DOWN;

			else if (GetKeyUp(MOUSE::LMOUSE))
				m_state = BUTTON_STATE::UP;

			else if (m_prevState == BUTTON_STATE::IDEL || m_prevState == BUTTON_STATE::OVER)
				m_state = BUTTON_STATE::OVER;
		}
		else
			m_state = BUTTON_STATE::IDEL;

		m_prevState = m_state;
	}
	else
		m_prevState = m_state = BUTTON_STATE::IDEL;
}

bool Truth::UI::IsActive()
{
	auto owner = m_owner.lock();

	bool isActive;
	bool isParentActive;
	isActive = owner->m_isActive;

	if (owner->HasParent())
		isParentActive = owner->m_parent.lock()->m_isActive;
	else
		isParentActive = true;

	return isActive && isParentActive;
}

void Truth::UI::SetSpriteActive(BUTTON_STATE _state)
{
	uint32 state = static_cast<uint32>(_state);
	for (uint32 i = 0; i < 3; i++)
	{
		if (state == i)
			m_sprite[i]->SetActive(true);
		else
			m_sprite[i]->SetActive(false);
	}
}

#ifdef EDITOR_MODE
void Truth::UI::EditorSetValue()
{
	auto gp = m_managers.lock()->Graphics();
	for (uint32 i = 0; i < 3; i++)
	{
		float w = static_cast<float>(m_texture[i]->w);
		float h = static_cast<float>(m_texture[i]->h);
		m_sprite[i]->SetScale({ m_size.x / w, m_size.y / h });
		m_sprite[i]->SetPosition(m_position);
		m_sprite[i]->SetActive(IsActive());
		m_sprite[i]->SetAlpha(m_alpha);
		m_sprite[i]->SetZ(m_zDepth);
	}

	Vector2 realLT = gp->GetContentPosMin();
	Vector2 realRB = gp->GetContentPosMax();

	float contentW = realRB.x - realRB.x;
	float contentH = realRB.y - realRB.y;

	RECT winRect = gp->GetWindowRect();

	float winW = static_cast<float>(winRect.right - winRect.left);
	float winH = static_cast<float>(winRect.bottom - winRect.top);

	float ratioW = contentW / winW;
	float ratioH = contentH / winH;

	Vector2 editorSize = {};
	editorSize.x = m_size.x * ratioW;
	editorSize.y = m_size.y * ratioW;

	Vector2 editorPos = {};
	editorPos.x = realLT.x + (m_position.x * ratioW);
	editorPos.y = realLT.y + (m_position.y * ratioW);

	m_rect.left = static_cast<LONG>(m_position.x - (editorSize.x * 0.5f));
	m_rect.top = static_cast<LONG>(m_position.y - (editorSize.y * 0.5f));
	m_rect.right = static_cast<LONG>(m_position.x + (editorSize.x * 0.5f));
	m_rect.bottom = static_cast<LONG>(m_position.y + (editorSize.y * 0.5f));
}
#endif // IFDE
