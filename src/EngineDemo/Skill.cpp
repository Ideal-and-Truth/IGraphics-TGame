#include "Skill.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Skill)

Skill::Skill()
	: m_id(0)
	, m_name("")
	, m_description("")
	, m_isUnlocked(false)
	, m_isActivated(false)
	, m_isSkillActivated(false)
	, m_unlockCost(0.f)
	, m_currentSkillLevel(0)
	, m_maxSkillLevel(0)
{
	m_name = "Skill";
}

Skill::~Skill()
{

}

void Skill::Awake()
{

}

void Skill::Update()
{
	// 해금됐는지 확인하고
	if (!m_isActivated)
	{
		int count = 0;
		for (auto& e : m_prevSkills)
		{
			if (e->GetTypeInfo().GetProperty("isSkillActivated")->Get<bool>(e.get()).Get())
			{
				count++;
			}
		}
		if (count == m_prevSkills.size())
		{
			m_isActivated = true;
		}
	}

	if (m_isActivated)
	{
		// 업글했는지 확인하고
		if (m_currentSkillLevel > 0 && (m_currentSkillLevel < m_maxSkillLevel) && m_isUnlocked && m_isSkillActivated)
		{
			m_currentSkillLevel++;
			m_isUnlocked = false;
		}

		// 스킬 찍었는지 확인하고
		if (m_isUnlocked && !m_isSkillActivated && (m_currentSkillLevel < m_maxSkillLevel))
		{
			m_currentSkillLevel++;
			m_isSkillActivated = true;
			m_isUnlocked = false;
		}
	}



}
