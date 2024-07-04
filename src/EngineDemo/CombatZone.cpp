#include "CombatZone.h"
#include "Collider.h"


BOOST_CLASS_EXPORT_IMPLEMENT(CombatZone)

CombatZone::CombatZone()
	: m_isTargetIn(false)
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
	m_owner.lock()->m_children;
}

void CombatZone::Update()
{
	m_target = Detect();
}

void CombatZone::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isTargetIn = true;
	}
}

void CombatZone::OnTriggerExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isTargetIn = false;
	}
}

std::shared_ptr<Truth::Entity> CombatZone::Detect()
{
	if (m_isTargetIn && m_target == nullptr)
	{
		std::vector<std::shared_ptr<Truth::Entity>> entities = m_managers.lock()->Scene()->m_currentScene->GetTypeInfo().GetProperty("entities")->Get<std::vector<std::shared_ptr<Truth::Entity>>>(m_managers.lock()->Scene()->m_currentScene.get()).Get();

		for (auto& e : entities)
		{
			if (e.get()->m_name == "Player")
			{
				return e;
			}
		}
	}

	return nullptr;
}
