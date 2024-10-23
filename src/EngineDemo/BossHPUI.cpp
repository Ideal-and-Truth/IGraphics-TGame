#include "BossHPUI.h"
#include "Entity.h"
#include "Enemy.h"
#include "UI.h"
BOOST_CLASS_EXPORT_IMPLEMENT(BossHPUI)

void BossHPUI::Update()
{
	if (!m_boss.expired())
	{
		float maxTP = m_boss.lock()->GetMaxTP();
		float currentTP = m_boss.lock()->GetCurrentTP();

		float rate = currentTP / maxTP;

		m_UI.lock()->SetScale({ rate, 1.0f }, true);

		float centerUV = 0.5f * rate;
		m_UI.lock()->SetSampling({ 0.5f - centerUV, 0.f }, { centerUV + 0.5f, 1.f });
	}
}

void BossHPUI::Start()
{
	m_boss = m_owner.lock()->m_parent.lock()->GetComponent<Enemy>();
	m_UI.lock()->SetOnlyUI();
}
