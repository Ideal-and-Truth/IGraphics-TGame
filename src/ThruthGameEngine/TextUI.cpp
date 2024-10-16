#include "TextUI.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Texture.h"
#include "IText.h"
#include "InputManager.h"
#include "ButtonBehavior.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::TextUI)

Truth::TextUI::TextUI()
	: m_textSprite(nullptr)
	, m_size{ 100.0f, 100.0f }
	, m_alpha(1.0f)
	, m_zDepth(0.0f)
	, m_position(100.0f, 100.0f)
	, m_behavior(nullptr)
	, m_fontSize(20)
	, m_text(L"test")
{
}

Truth::TextUI::~TextUI()
{
	m_managers.lock()->Graphics()->DeleteTextSprite(m_textSprite);
}

void Truth::TextUI::ChangeText(const std::wstring& _text)
{
	m_textSprite->ChangeText(_text);
}

void Truth::TextUI::Initialize()
{
	if (m_behavior)
		m_behavior->Initialize(m_managers.lock().get(), this, m_owner.lock().get());

	auto gp = m_managers.lock()->Graphics();
	m_textSprite = gp->CreateTextSprite(m_size.x, m_size.y, 20, m_text);
	m_textSprite->SetPosition(m_position);
	m_textSprite->SetActive(IsActive());
	m_textSprite->SetAlpha(m_alpha);
	m_textSprite->SetZ(m_zDepth);
}

void Truth::TextUI::Start()
{
	if (m_behavior)
		m_behavior->Start();
	ResizeWindow();
}

void Truth::TextUI::Awake()
{
	if (m_behavior)
		m_behavior->Awake();
}

void Truth::TextUI::Update()
{
#ifdef EDITOR_MODE
	ResizeWindow();
#endif
	if (m_behavior)
		m_behavior->Update();
}

bool Truth::TextUI::IsActive()
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

void Truth::TextUI::SetSpriteActive(bool _active)
{
	m_textSprite->SetActive(_active);
}

void Truth::TextUI::ResizeWindow()
{
	auto gp = m_managers.lock()->Graphics();

	// RECT winRect = gp->GetWindowRect();
	Vector2 realLT = gp->GetContentPosMin();
	Vector2 realRB = gp->GetContentPosMax();
	Vector2 resolution = gp->GetDisplayResolution();

	float contentW = realRB.x - realLT.x;
	float contentH = realRB.y - realLT.y;

	float winW = resolution.x;
	float winH = resolution.y;

	float ratioW = contentW / winW;
	float ratioH = contentH / winH;

	Vector2 editorSize = {};
	editorSize.x = m_size.x * ratioW;
	editorSize.y = m_size.y * ratioH;

	Vector2 editorPos = {};
	editorPos.x = realLT.x + (m_position.x * ratioW);
	editorPos.y = realLT.y + (m_position.y * ratioH);
}

#ifdef EDITOR_MODE
void Truth::TextUI::EditorSetValue()
{
	ResizeWindow();

	m_textSprite->SetScale({ m_size.x, m_size.y });
	m_textSprite->SetPosition({ m_position.x - (m_size.x * 0.5f), m_position.y - (m_size.y * 0.5f) });
	m_textSprite->SetActive(IsActive());
	m_textSprite->SetAlpha(m_alpha);
	m_textSprite->SetZ(m_zDepth);
	m_textSprite->ChangeText(m_text);
}
#endif // EDITOR_MODE
