#pragma once
#include "Component.h"


class BossAnimator;
class Player;

class BossSkill :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(BossSkill);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::shared_ptr<BossAnimator> m_bossAnimator;
	std::shared_ptr<Player> m_player;

	std::vector<std::pair<std::shared_ptr<Truth::Entity>, bool>> m_fires;
	std::vector<std::pair<std::shared_ptr<Truth::Entity>, bool>> m_shockWaves;
	std::vector<std::pair<std::shared_ptr<Truth::Entity>, bool>> m_swords;
	std::vector<std::pair<std::shared_ptr<Truth::Entity>, bool>> m_clones;
	std::vector<std::pair<std::shared_ptr<Truth::Entity>, bool>> m_timeSpheres;

	bool m_swordShooting;
	bool m_shockWave;
	bool m_flameSword;
	bool m_lightSpeedDash;
	bool m_timeDistortion;

	float m_swordShootCoolTime;
	float m_shockWaveCoolTime;
	float m_flameSwordCoolTime;
	float m_lightSpeedDashCoolTime;
	float m_timeDistortionCoolTime;

	bool m_deleteFire;
	bool m_deleteSword;
	bool m_deleteClone;
	bool m_deleteSphere;

	bool m_createComplete1;
	bool m_createComplete2;
	bool m_createComplete3;
	bool m_createComplete4;

	PROPERTY(createComplete5);
	bool m_createComplete5;

	bool m_paternEnds;
	bool m_readyToShoot;

	float m_shockWaveTime;
	float m_flameSwordTime;
	float m_swordShootTime;
	float m_lightSpeedDashTime;
	float m_timeDistortionTime;

	int m_shockCount;
	int m_swordCount;
	int m_flameCount;
	int m_cloneCount;

	int m_currentPhase;

	bool m_playShock;
	bool m_playSpear;
	int m_spearImpactCount;

	std::vector<float> m_shockWavePos;
	std::vector<float> m_flamePos;
	std::vector<Vector3> m_swordPos;
	std::vector<Vector3> m_shootingPos;
	std::vector<Vector3> m_illusionPos;
	std::vector<Vector3> m_spherePos;

public:
	BossSkill();
	virtual ~BossSkill();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();

private:
	void ShockWave();
	void FlameSword();
	void SwordShooting();
	void LightSpeedDash(bool isSecondPhase);
	void DistortedTimeSphere();

	int RandomNumber(int _min, int _max);

	void CoolTimeCheck();
	void DeleteCheck();

	void PlayEffect(Vector3 pos);
};

template<class Archive>
void BossSkill::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void BossSkill::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(BossSkill)
BOOST_CLASS_VERSION(BossSkill, 0)