#pragma once
#include "Component.h"
class StageBGM :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(StageBGM);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	bool m_isPlay;
	std::string m_sceneName;

public:
	StageBGM();
	virtual ~StageBGM();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();
};

template<class Archive>
void StageBGM::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void StageBGM::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(StageBGM)
BOOST_CLASS_VERSION(StageBGM, 0)