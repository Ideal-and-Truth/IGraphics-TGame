#pragma once
#include "Component.h"

class Skill
{
private:
	int m_id;
	std::string m_name;
	std::string m_description;
	// 해금됐는지
	bool m_isUnlocked;
	// 활성화됐는지
	bool m_isDisabled;
	std::vector<Skill> m_prevSkills;

public:
	Skill();
	~Skill();



};


class SkillTree :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(SkillTree);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:


public:
	SkillTree();
	virtual ~SkillTree();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();
};

template<class Archive>
void SkillTree::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void SkillTree::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(SkillTree)
BOOST_CLASS_VERSION(SkillTree, 0)