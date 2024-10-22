#include "TestEffectGenerator.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"
BOOST_CLASS_EXPORT_IMPLEMENT(TestEffectGenerator)

void TestEffectGenerator::Update()
{
	/// 블랙홀
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\BlackHole.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateRotationX(3.14 * 0.5)
// 		);
// 		p->SetActive(true);
// 		p->Play();
//   	}
// 
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\BlackHoleSphere.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		//p->SetTransformMatrix(Matrix::CreateScale(10.f,10.f,10.f));
// 		p->SetActive(true);
// 		//p->SetSimulationSpeed(0.1f);
// 		p->Play();
// 	}

	/// 불
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Fire.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			DirectX::SimpleMath::Matrix::CreateScale(Vector3(2, 2, 4))
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}
// 
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Fire_1.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(Matrix::CreateScale(Vector3(1, 1, 5))
// 			* DirectX::SimpleMath::Matrix::CreateTranslation(Vector3(0, 0, -1))
// 			);
// 		p->SetActive(true);
// 		p->Play();
// 	}

	/// 창
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\SpearImpact.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateScale(35.f)
// 			* Matrix::CreateScale(Vector3(2.5, 2.5, 1))
// 			* Matrix::CreateRotationX(3.1415 * 0.5)
// 			* Matrix::CreateTranslation(-3, 0, 0)
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}
// 
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\SpearImpact1.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateScale(35.f)
// 			* Matrix::CreateScale(Vector3(3, 3, 1))
// 			* Matrix::CreateRotationX(3.1415 * 0.5)
// 			* Matrix::CreateTranslation(-3, 0, 0)
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}
// 
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\SpearImpact2.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateScale(35.f)
// 			* Matrix::CreateScale(Vector3(1, 1, 2))
// 			* Matrix::CreateRotationX(3.1415 * 0.5)
// 			* Matrix::CreateTranslation(-3, 1, 0)
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}

	/// 회전베기
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack_S.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateScale(Vector3(1.5, 3, 1.5))
// 			* Matrix::CreateRotationZ(0.13f)
// 			* Matrix::CreateTranslation(Vector3(3, 0, 0))
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}
// 
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack_S.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateScale(Vector3(1.5, 3, 1.5))
// 			* Matrix::CreateRotationY(2.44f)
// 			* Matrix::CreateRotationZ(-0.13f)
// 			* Matrix::CreateTranslation(Vector3(3, 0, 0))
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}
// 
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack_S.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateRotationY(2.44f)
// 			* Matrix::CreateTranslation(Vector3(3, 0, 0))
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}

	/// 강화평타
//작은년
// 	if (GetKeyDown(KEY::O))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateRotationY(-2.7925f)
// 			//Matrix::CreateRotationY(-3.14f)
// 
// 			*Matrix::CreateTranslation(Vector3(6, 0, 0))
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}
// 
// 	if (GetKeyDown(KEY::O))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack2.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateScale(Vector3(1.5f, 1.2f, 1.5f))
// 			*Matrix::CreateRotationY(-3.14f)
// 			 *Matrix::CreateTranslation(Vector3(6, 0, 0))
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}

	/// 회피평타
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\DodgeAttack.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(
// 			Matrix::CreateRotationY(-2.44f)
// 			* Matrix::CreateTranslation(Vector3(9, 0, 0))
// 		);
// 		p->SetActive(true);
// 		p->Play();
// 	}

	/// 마법진
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\MagicCircle.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(Matrix::CreateScale(0.5f) 
// 			* Matrix::CreateRotationX(1.57f) 
// 			* Matrix::CreateTranslation(Vector3(0, 3, 0))
// 		);
// 
// 		p->SetActive(true);
// 		p->Play();
// 	}

	/// 발사
// 	if (GetKeyDown(KEY::P))
// 	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ShootImpact.yaml");
// 		p->SetDeltaTime(GetDeltaTime());
// 		p->SetTransformMatrix(Matrix::CreateScale(0.8f) 
// 			* Matrix::CreateRotationX(1.57f) 
// 			* Matrix::CreateRotationY(-1.57f) 
// 			* Matrix::CreateTranslation(Vector3(0, 3, -0.2f))
// 		);
// 
// 
// 		p->SetActive(true);
// 		p->Play();
// 	}

	/// 빔
	if (GetKeyDown(KEY::P))
	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam.yaml");
		p->SetDeltaTime(GetDeltaTime());
		p->SetTransformMatrix(
			DirectX::SimpleMath::Matrix::CreateScale(Vector3(0.4, 0.4, 1)) *
			DirectX::SimpleMath::Matrix::CreateRotationX(3.14f * 0.5f)
		);

		p->SetActive(true);
		p->Play();
	}

	if (GetKeyDown(KEY::P))
	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam1.yaml");
		p->SetDeltaTime(GetDeltaTime());
		p->SetTransformMatrix(
			DirectX::SimpleMath::Matrix::CreateScale(Vector3(0.4, 0.4, 1)) *
			DirectX::SimpleMath::Matrix::CreateRotationX(3.14f * 0.5f)
		);

		p->SetActive(true);
		p->Play();
	}

	if (GetKeyDown(KEY::P))
	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam2.yaml");
		p->SetDeltaTime(GetDeltaTime());
		

		p->SetActive(true);
		p->Play();
	}
}
