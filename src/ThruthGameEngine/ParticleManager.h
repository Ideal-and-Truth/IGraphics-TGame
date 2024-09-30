#pragma once

#include "Headers.h"

namespace Truth
{
	class GraphicsManager;
}

namespace Ideal
{
	class IParticleSystem;
}

namespace Truth
{
	class ParticleManager
	{
	private:
		std::shared_ptr<GraphicsManager> m_grapics;

		std::unordered_map<std::string, std::shared_ptr<Ideal::IParticleSystem>> m_particleMap;
		std::unordered_map<std::string, std::shared_ptr<Ideal::IParticleMaterial>> m_particleMatMap;

	public:
		ParticleManager();
		~ParticleManager();

		void Initalize(std::shared_ptr<GraphicsManager> _grapics);
		void Finalize();
		
		void CreateEmptyParticle();
		void LoadParticle(const std::string& _name, fs::path _path);
		void SaveParticle(const std::string& _name, std::shared_ptr<Ideal::IParticleSystem> _partice, fs::path _path);
	};
}

