#include "CutSceneTrigger.h"
#include "Collider.h"
#include "PlayerCamera.h"
#include "Camera.h"

BOOST_CLASS_EXPORT_IMPLEMENT(CutSceneTrigger)

CutSceneTrigger::CutSceneTrigger()
{
	m_name = "CutSceneTrigger";
}

void CutSceneTrigger::OnTriggerEnter(Truth::Collider* _other)
{
	if (m_wasActive)
		return;

	m_wasActive = true;

	if (_other == nullptr)
		return;

	if (_other->GetOwner().lock()->m_name == "Player")
	{
		for (auto& e : m_managers.lock()->Scene()->m_currentScene->m_entities)
		{
			m_pCamera = e->GetComponent<PlayerCamera>();
			if (!m_pCamera.expired())
			{
				m_mainCamera = m_pCamera.lock()->GetCamera();
				m_pCamera.lock()->SetCutScenePaly(true);
				Play();
				break;
			}
		}
	}
}

void CutSceneTrigger::Update()
{
	CineCamera::Update();
	if (!m_pCamera.expired() && m_isEnd)
	{
		m_pCamera.lock()->SetCutScenePaly(false);
		m_pCamera.reset();
	}
}
