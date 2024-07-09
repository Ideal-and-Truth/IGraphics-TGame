#include "CombatZone.h"
#include "Collider.h"
#include "Enemy.h"


BOOST_CLASS_EXPORT_IMPLEMENT(CombatZone)

CombatZone::CombatZone()
{

}

CombatZone::~CombatZone()
{

}

void CombatZone::Awake()
{

}

void CombatZone::Start()
{

}

void CombatZone::Update()
{

}

void CombatZone::OnTriggerEnter(Truth::Collider* _other)
{
	int a = 0;
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		for (auto& e : m_owner.lock()->m_children)
		{
			if (e.get()->GetComponent<Enemy>().lock())
			{
				e.get()->GetComponent<Enemy>().lock().get()->GetTypeInfo().GetProperty("isTargetIn")
					->Set(e.get()->GetComponent<Enemy>().lock().get(), true);
			}
		}
	}
}

void CombatZone::OnTriggerExit(Truth::Collider* _other)
{
	int a1 = 0;
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		for (auto& e : m_owner.lock()->m_children)
		{
			if (e.get()->GetComponent<Enemy>().lock())
			{
				e.get()->GetComponent<Enemy>().lock().get()->GetTypeInfo().GetProperty("isTargetIn")
					->Set(e.get()->GetComponent<Enemy>().lock().get(), false);
			}
		}
	}
}



