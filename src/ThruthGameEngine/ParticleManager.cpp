#include "ParticleManager.h"
#include "IParticleSystem.h"
#include <yaml-cpp/yaml.h>
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
}

void Truth::ParticleManager::Finalize()
{

}

void Truth::ParticleManager::CreateEmptyParticle()
{
	YAML::Node node;
	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	emitter << YAML::Key << "EBlendingMode" << YAML::Value << 0;
	emitter << YAML::Key << "Shader" << YAML::Value << "TestCustomParticle";
	emitter << YAML::Key << "Texture0" << YAML::Value << ".";
	emitter << YAML::Key << "Texture1" << YAML::Value << ".";
	emitter << YAML::Key << "Texture2" << YAML::Value << ".";
	emitter << YAML::Key << "Mesh" << YAML::Value << "0_Particle/Slash";

	emitter << YAML::Key << "StartColor" << YAML::BeginSeq <<
		YAML::Value << 0.0f <<
		YAML::Value << 0.0f <<
		YAML::Value << 0.0f <<
		YAML::Value << 0.0f <<
	YAML::EndSeq;

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

	emitter << YAML::Key << "GetCustomData1X" << YAML::BeginSeq <<
		YAML::BeginMap <<
			YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 1.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
			YAML::Key << "x" << YAML::Value << 1.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
	YAML::EndSeq;

	emitter << YAML::EndMap;

	fs::path p = "../Resources/Particles";

	p /= "EmptyParticle.yaml";

	std::ofstream fout(p);
	fout << emitter.c_str();
}

void Truth::ParticleManager::LoadParticle(const std::string& _name, fs::path _path)
{
	std::ifstream fin(_path);
	auto node = YAML::Load(fin);

	std::shared_ptr<Ideal::IParticleMaterial> mat = m_grapics->CreateParticleMaterial();
	std::shared_ptr<Ideal::IParticleSystem> particle = m_grapics->CreateParticle(mat);

	// mat->SetTexture0();
}

void Truth::ParticleManager::SaveParticle(const std::string& _name, std::shared_ptr<Ideal::IParticleSystem> _partice, fs::path _path)
{
}

