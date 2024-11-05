#pragma once
#include "Component.h"


class Player :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Player);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	PROPERTY(moveSpeed);
	float m_moveSpeed;

	PROPERTY(lookRotationDampFactor);
	float m_lookRotationDampFactor;

	PROPERTY(stopTiming);
	float m_stopTiming;

	PROPERTY(maxTP);
	float m_maxTP;

	PROPERTY(maxCP);
	float m_maxCP;

	PROPERTY(currentDamage);
	float m_currentDamage;

	PROPERTY(currentTP);
	float m_currentTP;

	PROPERTY(currentCP);
	float m_currentCP;

	PROPERTY(chargingCP);
	float m_chargingCP;

	PROPERTY(isDecreaseCP);
	bool m_isDecreaseCP;

	PROPERTY(unlockSkill1);
	bool m_unlockSkill1;

	float m_passingTime;

	float m_dotDealTime;

	bool m_isInvincible;

	PROPERTY(slowTime);
	bool m_slowTime;

	const static fs::path m_dataPath;

	PROPERTY(onFire);
	bool m_onFire;

	bool m_getDotDeal;

	float m_burnedTime;

public:
	Player();
	virtual ~Player();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

	inline float GetCurrentTP() const { return m_currentTP; };
	inline float GetMaxTP() const { return m_maxTP; }

	inline float GetCurrentCP() const { return m_currentCP; };
	inline float GetMaxCP() const { return m_maxCP; }

	inline bool GetSlowTime() const { return m_slowTime; }

	void SavePlayerData(int _slot = 0);
	void LoadPlayerData(int _slot = 0);

private:
};

template<class Archive>
void Player::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

	if (file_version >= 1)
	{
		_ar& m_moveSpeed;
		_ar& m_lookRotationDampFactor;
		_ar& m_stopTiming;
		_ar& m_maxTP;
		_ar& m_maxCP;
		_ar& m_currentDamage;
		_ar& m_currentTP;
		_ar& m_currentCP;
		_ar& m_chargingCP;
	}
}

template<class Archive>
void Player::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

	_ar& m_moveSpeed;
	_ar& m_lookRotationDampFactor;
	_ar& m_stopTiming;
	_ar& m_maxTP;
	_ar& m_maxCP;
	_ar& m_currentDamage;
	_ar& m_currentTP;
	_ar& m_currentCP;
	_ar& m_chargingCP;
}

BOOST_CLASS_EXPORT_KEY(Player)
BOOST_CLASS_VERSION(Player, 1)