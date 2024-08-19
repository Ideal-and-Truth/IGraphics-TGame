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
	// ���̵�
	int m_id;
	// �̸�
	std::string m_name;
	// ����
	std::string m_description;
	// ��ư ��������
	bool m_isUnlocked;
	// ��ų�� Ȱ��ȭ�ƴ���
	PROPERTY(isSkillActivated);
	bool m_isSkillActivated;
	// �رݵƴ���
	bool m_isActivated;
	// ���
	float m_unlockCost;
	// ���� ����
	int m_currentSkillLevel;
	// �ִ� ����
	int m_maxSkillLevel;
	// ���� �ܰ��
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