#include "MeleeTriggerEnterDamager.h"
#include "Collider.h"
#include "Damageable.h"

MeleeTriggerEnterDamager::MeleeTriggerEnterDamager()
{
	m_name = "MeleeTriggerEnterDamager";
}

MeleeTriggerEnterDamager::~MeleeTriggerEnterDamager()
{

}

void MeleeTriggerEnterDamager::Awake()
{

}

void MeleeTriggerEnterDamager::Update()
{

}

void MeleeTriggerEnterDamager::OnTriggerEnter(Truth::Collider* _other)
{
	if (!m_isAttack)
	{
		return;
	}

	if ("Player" == m_owner.lock()->GetTypeInfo().GetProperty("tag")->Get<std::string>(m_owner.lock().get()).Get())
	{

	}

	auto d = _other->GetOwner().lock().get()->GetComponent<Damageable>().lock().get();

	if (d == nullptr)
	{
		return;
	}

	if (d->GetOwner().lock().get() == m_owner.lock().get())
	{
		return;
	}

	auto msg = new Damageable::DamageMessage();

	msg->damager = this;
	msg->amount = m_damageAmount;
	msg->direction = { 0,1,0 };
	msg->damageSource;
	msg->throwing = false;
	msg->stopCamera = false;
	
	
	
}
