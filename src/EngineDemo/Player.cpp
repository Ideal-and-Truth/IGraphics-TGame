#include "Player.h"
#include "Collider.h"
#include "Managers.h"
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
	, m_isDead(false)
	, m_isCutScene(false)
{
	m_name = "Player";
}

Player::~Player()
{

}

void Player::Awake()
{
	EventBind("SaveData", &Player::SavePlayerData);
	EventBind("LoadData", &Player::LoadPlayerData);
}

void Player::Start()
{
	m_currentTP = m_maxTP;
}

void Player::Update()
{
	if (m_currentTP <= 0.f)
	{
		m_isDead = true;
	}

	if (m_isDead)
	{
		return;
	}


	/// ������ �Ǵ� ������ Ŀ�ǵ�
	if (GetKey(KEY::O) && GetKeyDown(KEY::P))
		m_isInvincible = !m_isInvincible;

	/// �ִ� ü�� �̻� �� �ö󰡰�
	if (m_currentCP > m_maxCP)
		m_currentCP = m_maxCP;

	/// ��Ʈ������ ���
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

	/// �ð� ���� Ű�� 10�ʿ� ���� CP ����
	if (m_slowTime)
	{
		m_currentCP -= GetDeltaTime() * 10.f;
		if (m_currentCP < 0.f)
		{
			m_currentCP = 0.f;
			m_slowTime = false;
		}
	}
	
	if (m_isCutScene)
	{
		return;
	}

	/// 1�ʸ��� ü�� ����
	if (m_currentTP >= 0.f)
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime >= 1.f)
		{
			m_currentTP -= 1.f;
			m_passingTime = 0.f;
		}
	}


	/// ����
	if (m_isInvincible)
	{
		m_currentTP = m_maxTP;
		return;
	}


}

void Player::SavePlayerData([[maybe_unused]] const void* _)
{
	::SetCurrentDirectory(Truth::Managers::GetRootPath().c_str());

	fs::path savePath = m_dataPath / "data.player";

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
	if (!fs::exists(savePath.parent_path()))
	{
		fs::create_directories(savePath.parent_path());
	}
	std::ofstream fout(savePath);
	fout << emitter.c_str();

	fout.close();
}

void Player::LoadPlayerData([[maybe_unused]] const void* _)
{
	fs::path savePath = m_dataPath / "data.player";
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

