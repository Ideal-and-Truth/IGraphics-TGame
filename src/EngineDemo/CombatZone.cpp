#include "CombatZone.h"
#include "Collider.h"


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
			e.get()->GetTypeInfo().GetProperty("isTargetIn")->Set(e.get(), true);
		}
	}
}

void CombatZone::OnTriggerExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		for (auto& e : m_owner.lock()->m_children)
		{
			e.get()->GetTypeInfo().GetProperty("isTargetIn")->Set(e.get(), false);
		}
	}
}



