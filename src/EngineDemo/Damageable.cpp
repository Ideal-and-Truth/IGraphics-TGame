#include "Damageable.h"
#include "Collider.h"

Damageable::Damageable()
{
	m_name = "Damageable";
}

Damageable::~Damageable()
{

}

void Damageable::Awake()
{

}

void Damageable::Start()
{
	m_collider = m_owner.lock().get()->GetComponent<Truth::Collider>().lock().get();
}

void Damageable::Update()
{

} 

void Damageable::ApplyDamage(DamageMessage InData)
{

}
