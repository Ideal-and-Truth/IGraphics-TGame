#include "PlayerSkill.h"
#include "Transform.h"
#include "SimpleDamager.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"
#include "SphereCollider.h"
#include "PlayerAnimator.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerSkill)


PlayerSkill::PlayerSkill()
	: m_useSwordBeam(false)
	, m_createComplete(false)
	, m_playSwordBeam(false)
	, m_swordBeamPos(0.f)
{
	m_name = "PlayerSkill";
}

PlayerSkill::~PlayerSkill()
{

}

void PlayerSkill::Start()
{
	m_playerAnimator = m_owner.lock()->GetComponent<PlayerAnimator>().lock();
}

void PlayerSkill::Update()
{
	if (m_playerAnimator->GetTypeInfo().GetProperty("swordBeam")->Get<bool>(m_playerAnimator.get()).Get())
	{
		m_useSwordBeam = true;
		m_playerAnimator->GetTypeInfo().GetProperty("swordBeam")->Set(m_playerAnimator.get(), false);
	}

	if (m_useSwordBeam)
	{
		SwordBeam();
	}
}

void PlayerSkill::SwordBeam()
{
	if (!m_createComplete)
	{
		// ����ü��
		std::shared_ptr<Truth::Entity> swordBeam = std::make_shared<Truth::Entity>(m_managers.lock());
		swordBeam->Initialize();
		swordBeam->m_layer = 1;
		swordBeam->AddComponent<Truth::SphereCollider>();
		auto damage = swordBeam->AddComponent<SimpleDamager>();
		damage->GetTypeInfo().GetProperty("damage")->Set(damage.get(), 30.f);
		damage->GetTypeInfo().GetProperty("user")->Set(damage.get(), m_owner.lock());

		swordBeam->m_name = "SwordBeam";
		m_managers.lock()->Scene()->m_currentScene->CreateEntity(swordBeam);
		m_owner.lock()->AddChild(swordBeam);

		swordBeam->SetPosition({ 0.f,0.f,-2.f });
		swordBeam->SetScale({ 2.f,5.f,5.f });

		swordBeam->Start();

		m_swordBeam = swordBeam;

		m_createComplete = true;
	}
	else
	{
		if (m_swordBeamPos <= 0.f)
		{
			m_playSwordBeam = true;
		}

		m_swordBeamPos += GetDeltaTime() * 0.8f;
		m_swordBeam->m_transform->m_position.y = m_owner.lock()->m_transform->m_position.y + 1.f;
		m_swordBeam->m_transform->m_position.z += -m_swordBeamPos;

		PlayEffect(m_swordBeam->GetWorldPosition());

		if (m_swordBeam->m_transform->m_position.z < -10.f)
		{
			m_swordBeamPos = 0.f;
			m_owner.lock()->DeleteChild(m_swordBeam);
			m_owner.lock().reset();
			m_swordBeam->Destroy();
			m_swordBeam = nullptr;
			m_useSwordBeam = false;
			m_createComplete = false;
		}
	}
}

void PlayerSkill::PlayEffect(Vector3 pos)
{
	Vector3 effectRot = m_owner.lock()->m_transform->m_rotation.ToEuler();
	{
		effectRot.z += (3.141592f / 180.f) * 90.f;
		effectRot.y += (3.141592f / 180.f) * 20.f;
		effectRot.x += (3.141592f / 180.f) * 90.f;

		Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack.yaml");
		p->SetTransformMatrix(
			rotationMT
			* Matrix::CreateTranslation(pos)
		);

		if (m_playSwordBeam)
		{
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}

	{
		Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack2.yaml");
		p->SetTransformMatrix(
			Matrix::CreateScale(Vector3(1.5f, 1.2f, 1.5f))
			* rotationMT
			* Matrix::CreateTranslation(pos)
		);

		if (m_playSwordBeam)
		{
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
			m_playSwordBeam = false;
		}
	}
}

