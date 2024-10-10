#pragma once

#include "Headers.h"
#include <yaml-cpp/yaml.h>
#include "IGraph.h"

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
	class ParticePool
	{
	public:
		std::vector<std::shared_ptr<Ideal::IParticleSystem>> m_pool;
		uint32 m_ind = 0;
		uint32 m_maxCount;
		std::shared_ptr<GraphicsManager> m_grapics;

	public:
		ParticePool(std::shared_ptr<GraphicsManager> _graphic);
		~ParticePool();

		std::shared_ptr<Ideal::IParticleSystem> GetParticle();
		void Reset();
	};


	class ParticleManager
	{
	private:
		std::shared_ptr<GraphicsManager> m_grapics;

		std::unordered_map<fs::path, std::shared_ptr<ParticePool>> m_particleMap;
		std::unordered_map<std::string, std::shared_ptr<Ideal::IParticleMaterial>> m_particleMatMap;

	public:
		ParticleManager();
		~ParticleManager();

		void Initalize(std::shared_ptr<GraphicsManager> _grapics);
		void Finalize();
		
		void CreateEmptyParticle();
		void LoadParticle(fs::path _path);
		void ReloadParticle(fs::path _path);
		void ReloadAllParticle();
		void ReloadAllParticle(fs::path _path);
		std::shared_ptr<Ideal::IParticleSystem> GetParticle(fs::path _path);

		void Reset();

	private:
		void GetControlPoints(const YAML::Node& _node, std::shared_ptr<Ideal::IParticleSystem> _particle, Ideal::IBezierCurve& _graph);
	};
}

