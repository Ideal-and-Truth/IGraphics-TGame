#include "TestEffectGenerator.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"
BOOST_CLASS_EXPORT_IMPLEMENT(TestEffectGenerator)

void TestEffectGenerator::Update()
{
	if (GetKeyDown(KEY::P))
	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\EmptyParticle.yaml");
		p->SetTransformMatrix(GetWorldTM());
		p->SetActive(true);
		p->Play();
		p->SetDeltaTime(GetDeltaTime());
	}
}
