#include "TimeDistortion.h"
#include "SkinnedMesh.h"
#include "Transform.h"
#include "Player.h"
#include "Controller.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"
#include "SoundManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(TimeDistortion)

TimeDistortion::TimeDistortion()
	: m_isFaster(false)
	, m_isPlayerIn(false)
	, m_active(false)
	, m_delete(false)
	, m_playCount(0)
	, m_count(0)
	, m_passingTime(0.f)
	, m_damage(0.f)
	, m_playerSpeed(0.f)
{
	m_name = "TimeDistortion";
}

TimeDistortion::~TimeDistortion()
{
	m_blackHole->SetActive(false);
	m_blackHoleRing->SetActive(false);
	m_managers.lock()->Sound()->Stop(67);
	m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_BlackHole_Bomb_Sound.wav", false, 68);

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam.yaml");
		p->SetTransformMatrix(
			Matrix::CreateScale(Vector3(0.8f, 0.8f, 1.f)) *
			Matrix::CreateRotationX(3.14f * 0.5f) *
			Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);

		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam1.yaml");
		p->SetTransformMatrix(
			Matrix::CreateScale(Vector3(0.8f, 0.8f, 1.f)) *
			Matrix::CreateRotationX(3.14f * 0.5f) *
			Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);

		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam2.yaml");
		p->SetTransformMatrix(
			Matrix::CreateScale(2.f) *
			Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition())
		);

		p->SetActive(true);
		p->Play();
	}
}

void TimeDistortion::Awake()
{
	m_playerEntity = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player").lock();
	m_playerMesh = m_playerEntity->GetComponent<Truth::SkinnedMesh>().lock();
	m_player = m_playerEntity->GetComponent<Player>().lock();
}

void TimeDistortion::Start()
{
	m_playerSpeed = m_player->GetTypeInfo().GetProperty("moveSpeed")->Get<float>(m_player.get()).Get();

	m_blackHole = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\BlackHoleSphere.yaml");
	m_blackHoleRing = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\BlackHole.yaml");

	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_BlackHole_Drop_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_BlackHole_Base_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_BlackHole_Bomb_Sound.wav", false);
}

void TimeDistortion::FixedUpdate()
{
	if (m_isPlayerIn)
	{
		auto con = m_playerEntity->GetComponent<Truth::Controller>().lock();
		con->Move(m_moveVec);
	}
	else
	{
		m_moveVec = Vector3::Zero;
	}
}

void TimeDistortion::Update()
{
	PlayEffect();

	if (m_active && !m_delete)
	{
		m_passingTime += GetDeltaTime();

		if (m_count > 4)
		{
			if (m_isPlayerIn)
			{
				m_playerMesh->SetAnimationSpeed(1.f);
				m_player->GetTypeInfo().GetProperty("moveSpeed")->Set(m_player.get(), m_playerSpeed);

				auto dir = m_owner.lock()->m_transform->m_position - m_playerEntity->m_transform->m_position;
				dir.Normalize();
				dir.y = -100.f;
				dir *= GetDeltaTime() * 5.f;
				m_moveVec = dir;
			}

			if (m_passingTime > 1.f)
			{
				auto currentTP = m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get();
				m_player->GetTypeInfo().GetProperty("currentTP")->Set(m_player.get(), currentTP - m_damage);
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
						m_player->GetTypeInfo().GetProperty("moveSpeed")->Set(m_player.get(), m_playerSpeed * 2.f);
						m_playerMesh->SetAnimationSpeed(2.f);
					}
					else
					{
						m_player->GetTypeInfo().GetProperty("moveSpeed")->Set(m_player.get(), m_playerSpeed * 0.5f);
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
		m_player->GetTypeInfo().GetProperty("moveSpeed")->Set(m_player.get(), m_playerSpeed);
	}
}


void TimeDistortion::PlayEffect()
{

	Vector3 effectPos = m_owner.lock()->m_transform->m_position;
	Vector3 effectRot = m_owner.lock()->m_transform->m_rotation.ToEuler();
	Matrix scaleMT = Matrix::CreateScale(m_owner.lock()->m_transform->m_scale);
	Matrix traslationMT = Matrix::CreateTranslation(effectPos);
	m_blackHole->SetTransformMatrix(scaleMT * traslationMT);
	//p->SetSimulationSpeed(0.5f);

	if (m_playCount == 0)
	{
		m_blackHole->SetActive(true);
		m_blackHole->Play();
		m_playCount++;
	}

	effectRot.x += (3.141592f / 180.f) * 90.f;

	Matrix scaleMT1 = Matrix::CreateScale(4.f, 4.f, 4.f);
	Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
	m_blackHoleRing->SetTransformMatrix(scaleMT1 * rotationMT * traslationMT);
	//p1->SetSimulationSpeed(0.1f);

	if (m_playCount == 1 && m_active)
	{
		m_playCount++;
	}

	if (m_playCount == 2)
	{
		m_blackHoleRing->SetActive(true);
		m_blackHoleRing->Play();
		m_playCount++;

		m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_BlackHole_Drop_Sound.wav", false, 66);
		m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_BlackHole_Base_Sound.wav", false, 67);
	}
}
