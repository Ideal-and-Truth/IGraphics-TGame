#pragma once
#include "Component.h"



class Player;

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
	std::weak_ptr<Truth::Entity> m_playerEntity;

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