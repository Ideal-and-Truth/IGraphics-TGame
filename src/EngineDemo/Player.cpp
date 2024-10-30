#include "Player.h"
#include "Collider.h"
#include <yaml-cpp/yaml.h>
BOOST_CLASS_EXPORT_IMPLEMENT(Player)

const fs::path Player::m_dataPath = "../PlayerData/";

Player::Player()
	: m_moveSpeed(3.f)
	, m_lookRotationDampFactor(10.f)
	, m_stopTiming(0.37f)
	, m_maxTP(100.f)
	, m_maxCP(100.f)
	, m_currentDamage(30.f)
	, m_currentTP(0.f)
	, m_currentCP(0.f)
	, m_chargingCP(10.f)
	, m_passingTime(0.f)
	, m_burnedTime(0.f)
	, m_dotDealTime(0.f)
	, m_isDecreaseCP(false)
	, m_unlockSkill1(false)
	, m_slowTime(false)
	, m_isInvincible(false)
	, m_onFire(false)
	, m_getDotDeal(false)
{
	m_name = "Player";
}

Player::~Player()
{

}

void Player::Awake()
{

}

void Player::Start()
{
	m_currentTP = m_maxTP;
}

void Player::Update()
{
	/// 무적이 되는 마법의 커맨드
	if (GetKey(KEY::O) && GetKeyDown(KEY::P))
		m_isInvincible = !m_isInvincible;

	/// 최대 체력 이상 못 올라가게
	if (m_currentCP > m_maxCP)
		m_currentCP = m_maxCP;

	/// 도트데미지 계산
	if (m_onFire)
	{
		m_getDotDeal = true;
	}

	if (m_getDotDeal)
	{
		m_dotDealTime += GetDeltaTime();

		if (m_dotDealTime > 0.2f)
		{
			m_dotDealTime = 0.f;

			m_currentTP -= 1.f;
		}

		if (!m_onFire)
		{
			m_burnedTime += GetDeltaTime();

			if (m_burnedTime > 3.f)
			{
				m_burnedTime = 0.f;
				m_getDotDeal = false;
			}
		}
	}

	/// 시간 감속 키면 10초에 걸쳐 CP 감소
	if (m_slowTime)
	{
		m_currentCP -= GetDeltaTime() * 10.f;
		if (m_currentCP < 0.f)
		{
			m_currentCP = 0.f;
			m_slowTime = false;
		}
	}

	/// 1초마다 체력 감소
	if (m_currentTP > 0.f)
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime >= 1.f)
		{
			m_currentTP -= 1.f;
			m_passingTime = 0.f;
		}
	}


	/// 무적
	if (m_isInvincible)
	{
		m_currentTP = m_maxTP;
		return;
	}


}

void Player::SavePlayerData(int _slot)
{
	fs::path savePath = m_dataPath / std::to_string(_slot) / ".player";

	YAML::Node node;
	YAML::Emitter emitter;
	emitter << YAML::BeginDoc;
	emitter << YAML::BeginMap;

	emitter << YAML::Key << "maxTP" << YAML::Value << m_maxTP;
	emitter << YAML::Key << "maxCP" << YAML::Value << m_maxCP;
	emitter << YAML::Key << "currentDamage" << YAML::Value << m_currentDamage;
	emitter << YAML::Key << "currentTP" << YAML::Value << m_currentTP;
	emitter << YAML::Key << "currentCP" << YAML::Value << m_currentCP;
	emitter << YAML::Key << "chargingCP" << YAML::Value << m_chargingCP;

	emitter << YAML::EndMap;
	emitter << YAML::EndDoc;

	std::ofstream fout(m_dataPath);
	fout << emitter.c_str();

	fout.close();
}

void Player::LoadPlayerData(int _slot)
{
	fs::path savePath = m_dataPath / std::to_string(_slot) / ".player";
	std::ifstream fin(savePath);
	auto node = YAML::Load(fin);

	m_maxTP = node["maxTP"].as<float>();
	m_maxCP = node["maxCP"].as<float>();
	m_currentDamage = node["currentDamage"].as<float>();
	m_currentTP = node["currentTP"].as<float>();
	m_currentCP = node["currentCP"].as<float>();
	m_chargingCP = node["chargingCP"].as<float>();

	fin.close();
}

