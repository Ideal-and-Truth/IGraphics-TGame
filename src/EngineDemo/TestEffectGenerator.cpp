#include "TestEffectGenerator.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"
BOOST_CLASS_EXPORT_IMPLEMENT(TestEffectGenerator)

void TestEffectGenerator::Update()
{
	if (GetKeyDown(KEY::P))
	{
// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\sphereImpact.yaml");
// 		p->SetTransformMatrix(Matrix::CreateScale(35.f)
// 			* Matrix::CreateScale(Vector3(2.5, 2.5, 1))
// 			* Matrix::CreateRotationX(3.1415 * 0.5)
// 			* Matrix::CreateTranslation(-3, 0, 0));
// 		p->SetActive(true);
// 		p->Play();
// 		p->SetDeltaTime(GetDeltaTime());

		m_managers.lock()->Scene()->ChangeScene("StageBoss");
	}
}
