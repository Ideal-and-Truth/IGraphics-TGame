#include "DotDamage.h"
#include "Player.h"
#include "PlayerAnimator.h"
#include "Enemy.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"

BOOST_CLASS_EXPORT_IMPLEMENT(DotDamage)


DotDamage::DotDamage()
	: m_playEffect(false)
{
	m_name = "DotDamage";
}

DotDamage::~DotDamage()
{
	m_player->GetTypeInfo().GetProperty("onFire")->Set(m_player.get(), false);
	auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\BossFireFloor.yaml");
	p->SetActive(false);
}

void DotDamage::Start()
{
	auto player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	m_player = player.lock()->GetComponent<Player>().lock();
}

void DotDamage::Update()
{
	PlayEffect();
}

void DotDamage::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_player->GetTypeInfo().GetProperty("onFire")->Set(m_player.get(), true);
	}
}

void DotDamage::OnTriggerExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_player->GetTypeInfo().GetProperty("onFire")->Set(m_player.get(), false);
	}
}

void DotDamage::PlayEffect()
{
	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\BossFireFloor.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(3.14 * 0.5)
			* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);

		if (m_playEffect)
		{
			m_playEffect = false;
			p->SetActive(true);
			p->Play();
		}
	}
}
