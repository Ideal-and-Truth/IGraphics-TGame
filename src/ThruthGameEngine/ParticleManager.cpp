#include "ParticleManager.h"
#include "IParticleSystem.h"
#include "GraphicsManager.h"
#include "IParticleMaterial.h"

Truth::ParticleManager::ParticleManager()
{

}

Truth::ParticleManager::~ParticleManager()
{

}

void Truth::ParticleManager::Initalize(std::shared_ptr<GraphicsManager> _grapics)
{
	m_grapics = _grapics;
	ReloadAllParticle();
}

void Truth::ParticleManager::Finalize()
{

}

void Truth::ParticleManager::CreateEmptyParticle()
{
	YAML::Node node;
	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	emitter << YAML::Key << "MaxCount" << YAML::Value << 10;
	emitter << YAML::Key << "MatName" << YAML::Value << "defaultMat";

	emitter << YAML::Key << "EBlendingMode" << YAML::Value << 0;
	emitter << YAML::Key << "Shader" << YAML::Value << "SwordSlash";
	emitter << YAML::Key << "Texture0" << YAML::Value << "../Resources/Textures/0_Particle/Smoke12.png";
	emitter << YAML::Key << "Texture1" << YAML::Value << "../Resources/Textures/0_Particle/Crater62.png";
	emitter << YAML::Key << "Texture2" << YAML::Value << "../Resources/Textures/0_Particle/Noise43b.png";
	emitter << YAML::Key << "Mesh" << YAML::Value << "0_Particle/Slash";

	emitter << YAML::Key << "StartColor" << YAML::BeginSeq <<
		YAML::Value << 1.0f <<
		YAML::Value << 1.0f <<
		YAML::Value << 1.0f <<
		YAML::Value << 1.0f <<
		YAML::EndSeq;

	emitter << YAML::Key << "StartLifetime" << YAML::Value << 1.0f;


	emitter << YAML::Key << "Loop" << YAML::Value << false;
	emitter << YAML::Key << "Duration" << YAML::Value << 2.0f;
	emitter << YAML::Key << "StopEnd" << YAML::Value << true;
	emitter << YAML::Key << "RenderMode" << YAML::Value << 1;
	emitter << YAML::Key << "RotationOverLifeTime" << YAML::Value << true;

	emitter << YAML::Key << "RotationOverLifetimeAxisYControlPoints" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 1.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.5f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 1.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData1X" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.1f << YAML::Key << "y" << YAML::Value << 2.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.3f << YAML::Key << "y" << YAML::Value << 0.6f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 1.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData1Y" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 1.0f << YAML::Key << "y" << YAML::Value << 1.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData2Z" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData2W" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 0.2f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "ColorOverLifetime" << YAML::Value << true;


	emitter << YAML::Key << "ColorOverLifetimeGradientGraph" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "Color" << YAML::BeginSeq << 
		YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 0.0f << 
		YAML::EndSeq <<
		YAML::Key << "position" << YAML::Value << 0.0f << 
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "Color" << YAML::BeginSeq <<
		YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 1.0f <<
		YAML::EndSeq <<
		YAML::Key << "position" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::EndMap;

	fs::path p = "..\\Resources\\Particles";

	p /= "EmptyParticle.yaml";

	std::ofstream fout(p);
	fout << emitter.c_str();
}

void Truth::ParticleManager::LoadParticle(fs::path _path)
{
	if (m_particleMap.find(_path) != m_particleMap.end())
	{
		return;
	}
	USES_CONVERSION;

	std::ifstream fin(_path);
	auto node = YAML::Load(fin);


	int maxCount = node["MaxCount"].as<int>();
	m_particleMap[_path] = std::make_shared<ParticePool>(m_grapics);
	// m_particleMap[_path]->m_pool.resize(maxCount);
	m_particleMap[_path]->m_maxCount = maxCount;

	for (int i = 0; i < maxCount; i++)
	{
		std::shared_ptr<Ideal::IParticleMaterial> mat = m_grapics->CreateParticleMaterial();

		std::wstring sahder = A2W(node["Shader"].as<std::string>().c_str());
		mat->SetShader(m_grapics->CreateShader(sahder));

		std::shared_ptr<Ideal::IParticleSystem> particle;

		std::string matName = node["MatName"].as<std::string>();
		if (m_particleMatMap.find(matName) != m_particleMatMap.end())
		{
			particle = m_grapics->CreateParticle(m_particleMatMap[matName]);
		}
		else
		{
			/// Create Texture
			if (node["Texture0"].IsDefined())
			{
				std::wstring tPath = A2W(node["Texture0"].as<std::string>().c_str());
				std::shared_ptr<Texture> t0 = m_grapics->CreateTexture(tPath);
				mat->SetTexture0(t0->m_texture);
			}
			if (node["Texture0"].IsDefined())
			{
				std::wstring tPath = A2W(node["Texture1"].as<std::string>().c_str());
				std::shared_ptr<Texture> t1 = m_grapics->CreateTexture(tPath);
				mat->SetTexture1(t1->m_texture);
			}

			if (node["Texture0"].IsDefined())
			{
				std::wstring tPath = A2W(node["Texture2"].as<std::string>().c_str());
				std::shared_ptr<Texture> t2 = m_grapics->CreateTexture(tPath);
				mat->SetTexture2(t2->m_texture);
			}
			if (node["EBlendingMode"].IsDefined())
			{
				mat->SetBlendingMode(static_cast<Ideal::ParticleMaterialMenu::EBlendingMode>(node["EBlendingMode"].as<int>()));
			}
			m_particleMatMap[matName] = mat;
			particle = m_grapics->CreateParticle(mat);
			particle->SetActive(false);
			particle->SetTransformMatrix(Matrix::Identity);
		}
		if (node["Mesh"].IsDefined())
		{
			particle->SetRenderMesh(m_grapics->CreateParticleMesh(A2W(node["Mesh"].as<std::string>().c_str())));
		}

		/// Create Particle 
		if (node["Loop"].IsDefined())
		{
			particle->SetLoop(node["Loop"].as<bool>());
		}
		if (node["Duration"].IsDefined())
		{
			particle->SetDuration(node["Duration"].as<float>());
		}
		if (node["StartColor"].IsDefined())
		{
			const YAML::Node& cNode = node["StartColor"];
			Color c(cNode[0].as<float>(), cNode[1].as<float>(), cNode[2].as<float>(), cNode[3].as<float>());
			particle->SetStartColor(c);
		}

		if (node["StartLifetime"].IsDefined())
		{
			particle->SetStartLifetime(node["StartLifetime"].as<float>());
		}
		if (node["RenderMode"].IsDefined())
		{
			particle->SetRenderMode(static_cast<Ideal::ParticleMenu::ERendererMode>(node["RenderMode"].as<int>()));
		}

		if (node["RotationOverLifeTime"].IsDefined())
		{
			particle->SetRotationOverLifetime(node["RotationOverLifeTime"].as<bool>());
		}

		if (node["RotationOverLifetimeAxisXControlPoints"].IsDefined())
		{
			const YAML::Node& child = node["RotationOverLifetimeAxisXControlPoints"];
			GetControlPoints(child, particle, particle->GetRotationOverLifetimeAxisX());
		}
		if (node["RotationOverLifetimeAxisYControlPoints"].IsDefined())
		{
			const YAML::Node& child = node["RotationOverLifetimeAxisYControlPoints"];
			GetControlPoints(child, particle, particle->GetRotationOverLifetimeAxisY());
		}
		if (node["RotationOverLifetimeAxisZControlPoints"].IsDefined())
		{
			const YAML::Node& child = node["RotationOverLifetimeAxisZControlPoints"];
			GetControlPoints(child, particle, particle->GetRotationOverLifetimeAxisZ());
		}

		if (node["CustomData1X"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1X"];
			GetControlPoints(child, particle, particle->GetCustomData1X());
		}
		if (node["CustomData1Y"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1Y"];
			GetControlPoints(child, particle, particle->GetCustomData1Y());
		}
		if (node["CustomData1Z"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1Z"];
			GetControlPoints(child, particle, particle->GetCustomData1Z());
		}
		if (node["CustomData1W"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1W"];
			GetControlPoints(child, particle, particle->GetCustomData1W());
		}

		if (node["CustomData2X"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2X"];
			GetControlPoints(child, particle, particle->GetCustomData2X());
		}
		if (node["CustomData2Y"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2Y"];
			GetControlPoints(child, particle, particle->GetCustomData2Y());
		}
		if (node["CustomData2Z"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2Z"];
			GetControlPoints(child, particle, particle->GetCustomData2Z());
		}
		if (node["CustomData2W"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2W"];
			GetControlPoints(child, particle, particle->GetCustomData2W());
		}

		if (node["ColorOverLifetime"].IsDefined())
		{
			particle->SetColorOverLifetime(node["ColorOverLifetime"].as<bool>());
		}

		if (node["ColorOverLifetimeGradientGraph"].IsDefined())
		{
			const YAML::Node& child = node["ColorOverLifetimeGradientGraph"];
			auto& graph = particle->GetColorOverLifetimeGradientGraph();
			for (int j = 0; j < 2; j++)
			{
				const YAML::Node& point = child[j]["Color"];
				Color startColor(point[0].as<float>(), point[1].as<float>(), point[2].as<float>(), point[3].as<float>());
				float position = child[j]["position"].as<float>();
				graph.AddPoint(startColor, position);
			}
		}

		m_particleMap[_path]->m_pool.push_back(particle);

	}
}

void Truth::ParticleManager::ReloadParticle(fs::path _path)
{

}

void Truth::ParticleManager::ReloadAllParticle()
{
	fs::path dir = "..\\Resources\\Particles";

	fs::directory_iterator itr(dir);
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path childPath = entry.path();

		if (fs::is_directory(childPath))
		{
			ReloadAllParticle(childPath);
		}
		else if (fs::is_regular_file(childPath))
		{
			LoadParticle(childPath);
		}
		itr++;
	}
}

void Truth::ParticleManager::ReloadAllParticle(fs::path _path)
{
	fs::directory_iterator itr(_path);
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path childPath = entry.path();

		if (fs::is_directory(childPath))
		{
			ReloadAllParticle(childPath);
		}
		else if (fs::is_regular_file(childPath))
		{
			LoadParticle(childPath);
		}
		itr++;
	}
}

std::shared_ptr<Ideal::IParticleSystem> Truth::ParticleManager::GetParticle(fs::path _path)
{
	return m_particleMap[_path]->GetParticle();
}

void Truth::ParticleManager::Reset()
{
	for (auto& pool : m_particleMap)
	{
		pool.second->Reset();
	}
	m_particleMap.clear();

	for (auto& mat : m_particleMatMap)
	{
		m_grapics->DeleteParticleMaterial(mat.second);
	}
	m_particleMatMap.clear();
}

void Truth::ParticleManager::GetControlPoints(const YAML::Node& _node, std::shared_ptr<Ideal::IParticleSystem> _particle, Ideal::IBezierCurve& _graph)
{
	for (YAML::const_iterator it = _node.begin(); it != _node.end(); ++it)
	{
		_graph.AddControlPoint(Ideal::Point((*it)["x"].as<float>(), (*it)["y"].as<float>()));
	}
}

Truth::ParticePool::ParticePool(std::shared_ptr<GraphicsManager> _graphic)
	: m_grapics(_graphic)
	, m_ind(0)
	, m_maxCount(10)
{

}

Truth::ParticePool::~ParticePool()
{
	Reset();
}

std::shared_ptr<Ideal::IParticleSystem> Truth::ParticePool::GetParticle()
{
	if (m_ind >= m_maxCount)
	{
		m_ind = 0;
	}
	return m_pool[m_ind++];
}

void Truth::ParticePool::Reset()
{
	for (auto& p : m_pool)
	{
		m_grapics->DeleteParticle(p);
	}
}
