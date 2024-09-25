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
	std::vector<std::shared_ptr<Truth::Entity>> m_attackColliders;

	bool m_useSkill;
	bool m_createComplete;
	bool m_paternEnds;
	float m_passingTime;
	int m_count;
	std::vector<float> m_shockWavePos;

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
	void LightSpeedDash(bool isThirdPhase);
	void DistortedTimeSphere();
	void DamageforPlayer(float damage);
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