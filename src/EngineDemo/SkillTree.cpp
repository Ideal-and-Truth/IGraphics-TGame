#include "SkillTree.h"
#include "Player.h"

BOOST_CLASS_EXPORT_IMPLEMENT(SkillTree)

SkillTree::SkillTree()
{
	m_name = "SkillTree";
}

SkillTree::~SkillTree()
{

}

void SkillTree::Awake()
{
	m_playerEntity = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
}

void SkillTree::Update()
{
	auto playerComponent = m_playerEntity.lock()->GetComponent<Player>().lock();

	playerComponent->GetTypeInfo().GetProperty("unlockSkill1")->Get<bool>(playerComponent.get()).Get();
	playerComponent->GetTypeInfo().GetProperty("unlockSkill1")->Set(playerComponent.get(), true);

}
