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
	m_particle->SetActive(false);
}

void DotDamage::Start()
{
	auto player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	m_player = player.lock()->GetComponent<Player>().lock();
	m_particle = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\BossFireFloor.yaml");
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
		m_particle->SetTransformMatrix(
			Matrix::CreateRotationX(3.14f * 0.5f)
			* Matrix::CreateScale(3.f)
			* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);

		if (m_playEffect)
		{
			m_playEffect = false;
			m_particle->SetActive(true);
			m_particle->Play();
		}
	}
}
