#include "Bullet.h"
#include "Player.h"
#include "PlayerAnimator.h"

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

void Bullet::Awake()
{

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
			player->GetTypeInfo().GetProperty("currentTP")->Set(player.get(), playerHp - m_bulletDamage);
		}
		m_isHit = true;
	}
}

