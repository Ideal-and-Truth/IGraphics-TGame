#include "MapMoveTrigger.h"
#include "Collider.h"
#include "Entity.h"

BOOST_CLASS_EXPORT_IMPLEMENT(MapMoveTrigger)

MapMoveTrigger::MapMoveTrigger()
	: m_nextMap("Stage2")
{
	m_nextMap;
}

MapMoveTrigger::~MapMoveTrigger()
{

}

void MapMoveTrigger::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other == nullptr)
		return;

	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_managers.lock()->Scene()->ChangeScene(m_nextMap);
	}
}
