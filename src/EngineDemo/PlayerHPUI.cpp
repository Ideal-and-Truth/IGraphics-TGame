#include "PlayerHPUI.h"
#include "Entity.h"
#include "Player.h"
#include "UI.h"
#include "GraphicsManager.h"
#include "ISprite.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerHPUI)

PlayerHPUI::PlayerHPUI()
{
	m_name = "Player Cp Behavior";
}

PlayerHPUI::~PlayerHPUI()
{
	auto gp = m_managers.lock()->Graphics();
	if (m_underSprite)
	{
		gp->DeleteSprite(m_underSprite);
		m_underSprite.reset();
	}
	if (m_upSprite)
	{
		gp->DeleteSprite(m_upSprite);
		m_upSprite.reset();
	}
}

void PlayerHPUI::Update()
{
	if (!m_player.expired())
	{
		float maxTP = m_player.lock()->GetMaxTP();
		float currentTP = m_player.lock()->GetCurrentTP();
		float rate;

		if (maxTP == 0.f)
			rate = 0.f;
		else
			rate = currentTP / maxTP;

		if (currentTP < 0)
			rate = 0.f;

		if (rate >= 0.0f && rate < 1.0f)
		{
			m_underSprite->SetTexture(m_HPTexture[0]->m_texture);
			m_upSprite->SetTexture(m_HPTexture[1]->m_texture);
		}
		if (rate >= 1.0f && rate < 2.0f)
		{
			m_underSprite->SetTexture(m_HPTexture[1]->m_texture);
			m_upSprite->SetTexture(m_HPTexture[2]->m_texture);
		}
		if (rate >= 2.0f && rate < 3.0f)
		{
			m_underSprite->SetTexture(m_HPTexture[2]->m_texture);
			m_upSprite->SetTexture(m_HPTexture[3]->m_texture);
		}
		if (rate >= 3.0f && rate < 4.0f)
		{
			m_underSprite->SetTexture(m_HPTexture[3]->m_texture);
			m_upSprite->SetTexture(m_HPTexture[4]->m_texture);
		}
		if (rate >= 4.0f && rate < 5.0f)
		{
			m_underSprite->SetTexture(m_HPTexture[4]->m_texture);
			m_upSprite->SetTexture(m_HPTexture[5]->m_texture);
		}

		SetSampling({ 0.f, 0.f }, { (rate - (int)(rate)), 1.f});
		SetScale({ (rate - (int)(rate)), 1.0f });
	}
}

void PlayerHPUI::Start()
{
	auto gp = m_managers.lock()->Graphics();

	m_HPTexture[5] = gp->CreateTexture(L"../Resources/Textures/UI/Player/TP_5.png", false, false, true);
	m_HPTexture[4] = gp->CreateTexture(L"../Resources/Textures/UI/Player/TP_4.png", false, false, true);
	m_HPTexture[3] = gp->CreateTexture(L"../Resources/Textures/UI/Player/TP_3.png", false, false, true);
	m_HPTexture[2] = gp->CreateTexture(L"../Resources/Textures/UI/Player/TP_2.png", false, false, true);
	m_HPTexture[1] = gp->CreateTexture(L"../Resources/Textures/UI/Player/TP_1.png", false, false, true);
	m_HPTexture[0] = gp->CreateTexture(L"../Resources/Textures/UI/Player/TP_Base.png", false, false, true);

	m_player = m_owner.lock()->m_parent.lock()->GetComponent<Player>();

	m_UI.lock()->SetOnlyUI();
	m_UI.lock()->m_noneUpdate = true;

	float w = static_cast<float>(m_HPTexture[0]->w);
	float h = static_cast<float>(m_HPTexture[0]->h);

	Vector2 gpScale = { (m_size.x / w), (m_size.y / h) };
	Vector2 gpPosition = { m_position.x - (m_size.x * 0.5f), m_position.y - (m_size.y * 0.5f) };

	m_underSprite = gp->CreateSprite();
	m_underSprite->SetTexture(m_HPTexture[0]->m_texture);
	m_underSprite->SetScale(gpScale);
	m_underSprite->SetPosition(gpPosition);
	m_underSprite->SetActive(true);
	m_underSprite->SetAlpha(1.0f);
	m_underSprite->SetZ(0.2f);
	m_underSprite->SetSampleRect(
		{
			static_cast<uint32>(w),
			static_cast<uint32>(h),
			static_cast<uint32>(w),
			static_cast<uint32>(h)
		});

	m_upSprite = gp->CreateSprite();
	m_upSprite->SetTexture(m_HPTexture[0]->m_texture);
	m_upSprite->SetScale(gpScale);
	m_upSprite->SetPosition(gpPosition);
	m_upSprite->SetActive(true);
	m_upSprite->SetAlpha(1.0f);
	m_upSprite->SetZ(0.1f);
	m_upSprite->SetSampleRect(
		{
			static_cast<uint32>(w),
			static_cast<uint32>(h),
			static_cast<uint32>(w),
			static_cast<uint32>(h)
		});
}

void PlayerHPUI::SetSampling(const Vector2& _min, const Vector2& _max)
{
	auto tex = m_HPTexture[0];
	float w = static_cast<float>(tex->w);
	float h = static_cast<float>(tex->h);

	m_upSprite->SetSampleRect(
		{
			static_cast<uint32>(w * _min.x),
			static_cast<uint32>(h * _min.y),
			static_cast<uint32>(w * _max.x),
			static_cast<uint32>(h * _max.y)
		});

	m_underSprite->SetSampleRect(
		{
			static_cast<uint32>(0),
			static_cast<uint32>(0),
			static_cast<uint32>(w),
			static_cast<uint32>(h)
		});
}

void PlayerHPUI::SetScale(const Vector2& _scale)
{
	auto tex = m_HPTexture[0];

	float w = static_cast<float>(tex->w);
	float h = static_cast<float>(tex->h);

	Vector2 gpScale = { (m_size.x / w) * _scale.x, (m_size.y / h) * _scale.y };
	Vector2 gpPosition = { (m_size.x / w) * _scale.x, (m_size.y / h) * _scale.y };
	m_upSprite->SetScale({ (m_size.x / w) * _scale.x, (m_size.y / h) * _scale.y });
	m_underSprite->SetScale({ (m_size.x / w), (m_size.y / h) });
}
