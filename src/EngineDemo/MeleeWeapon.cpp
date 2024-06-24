#include "MeleeWeapon.h"
#include "BoxCollider.h"

BOOST_CLASS_EXPORT_IMPLEMENT(MeleeWeapon)

MeleeWeapon::MeleeWeapon()
{
	m_name = "MeleeWeapon";
}

MeleeWeapon::~MeleeWeapon()
{

}

void MeleeWeapon::Awake()
{

}

void MeleeWeapon::Start()
{
	m_collider = m_owner.lock().get()->GetComponent<Truth::BoxCollider>().lock();
}

void MeleeWeapon::Update()
{

}
