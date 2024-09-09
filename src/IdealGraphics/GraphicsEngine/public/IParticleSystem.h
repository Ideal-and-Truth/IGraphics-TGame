#pragma once
#include <memory>

namespace Ideal
{
	class IParticleMaterial;
}

namespace Ideal
{
	class IParticleSystem
	{
	public:
		IParticleSystem() {};
		virtual ~IParticleSystem() {};

		// 복사 생성자를 만들어야 할지도
		// ON, OFF 기능도
	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) abstract;
	};
}