#pragma once
#include "Component.h"
class Skill :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Skill);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	// 아이디
	int m_id;
	// 이름
	std::string m_name;
	// 설명
	std::string m_description;
	// 버튼 눌렀는지
	bool m_isUnlocked;
	// 스킬이 활성화됐는지
	PROPERTY(isSkillActivated);
	bool m_isSkillActivated;
	// 해금됐는지
	bool m_isActivated;
	// 비용
	float m_unlockCost;
	// 현재 레벨
	int m_currentSkillLevel;
	// 최대 레벨
	int m_maxSkillLevel;
	// 이전 단계들
	std::vector<std::shared_ptr<Skill>> m_prevSkills;


public:
	Skill();
	virtual ~Skill();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();
};

template<class Archive>
void Skill::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void Skill::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(Skill)
BOOST_CLASS_VERSION(Skill, 0)