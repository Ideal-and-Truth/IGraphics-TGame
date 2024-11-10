#include "MapMoveTrigger.h"
#include "Collider.h"
#include "Entity.h"
#include "IParticleSystem.h"
#include "ParticleManager.h"
#include "Transform.h"

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
		EventPublish("Scene Move", nullptr);
		// m_managers.lock()->Scene()->ChangeScene(m_nextMap);
	}
}

void MapMoveTrigger::Start()
{
	PortalEffect();
}

void MapMoveTrigger::Update()
{
	if (GetKeyDown(KEY::F8))
		m_managers.lock()->Scene()->ChangeScene(m_nextMap);

}

void MapMoveTrigger::PortalEffect()
{
	Vector3 effPos = m_owner.lock()->GetWorldPosition();
	effPos.y -= 0.1f;
	Vector3 localPos = { 0.f,0.f,-1.f };

	auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Portal0.yaml");
	p->SetTransformMatrix(
		Matrix::CreateRotationX(1.57f)
		* Matrix::CreateFromQuaternion(m_owner.lock()->m_transform->m_rotation)
		* Matrix::CreateTranslation(localPos)
		* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
	);

	auto p1 = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Portal1.yaml");
	p1->SetTransformMatrix(
		Matrix::CreateRotationX(1.57f)
		* Matrix::CreateFromQuaternion(m_owner.lock()->m_transform->m_rotation)
		* Matrix::CreateTranslation(localPos)
		* Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
	);

	p->SetActive(true);
	p->Play();

	p1->SetActive(true);
	p1->Play();
}
