#include "TimeDistortion.h"
#include "SkinnedMesh.h"
#include "Transform.h"
#include "Player.h"
#include "Controller.h"

BOOST_CLASS_EXPORT_IMPLEMENT(TimeDistortion)

TimeDistortion::TimeDistortion()
	: m_isFaster(false)
	, m_isPlayerIn(false)
	, m_active(false)
	, m_delete(false)
	, m_count(0)
	, m_passingTime(0.f)
	, m_damage(0.f)
{
	m_name = "TimeDistortion";
}

TimeDistortion::~TimeDistortion()
{

}

void TimeDistortion::Awake()
{
	m_playerEntity = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player").lock();
	m_playerMesh = m_playerEntity->GetComponent<Truth::SkinnedMesh>().lock();
}

void TimeDistortion::Update()
{
	if (m_active)
	{
		m_passingTime += GetDeltaTime();

		if (m_count > 4)
		{
			if (m_isPlayerIn)
			{
				m_playerMesh->SetAnimationSpeed(1.f);

				auto con = m_playerEntity->GetComponent<Truth::Controller>().lock();
				auto dir = m_owner.lock()->m_transform->m_position - m_playerEntity->m_transform->m_position;
				dir.Normalize();
				dir.y = -100.f;
				dir *= GetDeltaTime();
				con->Move(dir);
			}

			if (m_passingTime > 4.f)
			{
				auto player = m_playerEntity->GetComponent<Player>().lock();
				auto currentTP = player->GetTypeInfo().GetProperty("currentTP")->Get<float>(player.get()).Get();
				player->GetTypeInfo().GetProperty("currentTP")->Set(player.get(), currentTP - m_damage);
				m_delete = true;
			}

		}
		else
		{
			if (m_passingTime > 3.f)
			{
				if (m_isPlayerIn)
				{
					if (m_count % 2 == 0)
					{
						m_playerMesh->SetAnimationSpeed(2.f);
					}
					else
					{
						m_playerMesh->SetAnimationSpeed(0.5f);
					}
				}

				m_count++;
				m_passingTime = 0.f;
			}

			if (m_count > 4)
			{
				m_passingTime = 0.f;
			}
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
