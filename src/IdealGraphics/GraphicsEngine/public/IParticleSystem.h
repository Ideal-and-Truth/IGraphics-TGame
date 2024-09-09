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

		// ���� �����ڸ� ������ ������
		// ON, OFF ��ɵ�
	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) abstract;
	};
}