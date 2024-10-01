#include "TestEffectGenerator.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"
BOOST_CLASS_EXPORT_IMPLEMENT(TestEffectGenerator)

void TestEffectGenerator::Update()
{
	auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\EmptyParticle.yaml");
	if (GetKeyDown(KEY::P))
	{
		p->SetActive(true);
		p->Play();
	}
	p->SetDeltaTime(GetDeltaTime());
}
