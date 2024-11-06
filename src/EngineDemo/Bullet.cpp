#include "Bullet.h"
#include "Player.h"
#include "PlayerAnimator.h"
#include "BoxCollider.h"
#include "PhysicsManager.h"
#include "RigidBody.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"
#include "SoundManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Bullet)


Bullet::Bullet()
	: m_bulletDamage(0.f)
	, m_isHit(false)
{
	m_name = "Bullet";
}

Bullet::~Bullet()
{

}

void Bullet::Start()
{
	m_owner.lock()->GetComponent<Truth::RigidBody>().lock()->AddImpulse(m_power);
	m_owner.lock()->GetComponent<Truth::RigidBody>().lock()->m_useGravity = false;
}

void Bullet::Update()
{

}

void Bullet::OnCollisionEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player" && !m_isHit)
	{
		auto player = _other->GetOwner().lock()->GetComponent<Player>().lock();
		auto playerAnimator = _other->GetOwner().lock()->GetComponent<PlayerAnimator>().lock();
		float playerHp = player->GetTypeInfo().GetProperty("currentTP")->Get<float>(player.get()).Get();

		if (!playerAnimator->GetTypeInfo().GetProperty("isDodge")->Get<bool>(playerAnimator.get()).Get())
		{
			float damage = m_bulletDamage;
			if (playerAnimator->GetTypeInfo().GetProperty("isGuard")->Get<bool>(playerAnimator.get()).Get()
				&& !playerAnimator->GetTypeInfo().GetProperty("parry")->Get<bool>(playerAnimator.get()).Get())
			{
				damage *= 0.3f;
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\02 Combat_Sound\\Player_Block_Sound_1.wav", true, 63);
			}
			if (playerAnimator->GetTypeInfo().GetProperty("parry")->Get<bool>(playerAnimator.get()).Get())
			{
				damage = 0.f;
			}

			player->GetTypeInfo().GetProperty("currentTP")->Set(player.get(), playerHp - m_bulletDamage);
		}
		m_isHit = true;
		PlayEffect();
	}
}

void Bullet::PlayEffect()
{
	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage0.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage0.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f) * Matrix::CreateRotationY(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage1.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage1.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f) * Matrix::CreateRotationY(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage2.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);
		p->SetActive(true);
		p->Play();
	}
}