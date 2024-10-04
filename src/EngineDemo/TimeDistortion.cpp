#include "TimeDistortion.h"
#include "SkinnedMesh.h"

BOOST_CLASS_EXPORT_IMPLEMENT(TimeDistortion)

TimeDistortion::TimeDistortion()
	: m_isFaster(false)
	, m_isPlayerIn(false)
{
	m_name = "TimeDistortion";
}

TimeDistortion::~TimeDistortion()
{

}

void TimeDistortion::Awake()
{
	auto player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	m_playerMesh = player.lock()->GetComponent<Truth::SkinnedMesh>().lock();
}

void TimeDistortion::Update()
{
	if (m_isPlayerIn)
	{
		if (m_isFaster)
		{
			m_playerMesh->SetAnimationSpeed(2.f);
		}
		else
		{
			m_playerMesh->SetAnimationSpeed(0.5f);
		}
	}
}

void TimeDistortion::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isPlayerIn = true;
	}
}

void TimeDistortion::OnTriggerExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isPlayerIn = false;
		m_playerMesh->SetAnimationSpeed(1.f);
	}
}
