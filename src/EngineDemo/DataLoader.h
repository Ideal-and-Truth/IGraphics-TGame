#pragma once
#include "Component.h"


class DataLoader :
    public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(DataLoader);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

public:
	DataLoader();
	virtual ~DataLoader();

private:
	METHOD(Awake);
	virtual void Awake();

};

template<class Archive>
void DataLoader::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);
}

template<class Archive>
void DataLoader::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}


BOOST_CLASS_EXPORT_KEY(DataLoader)
BOOST_CLASS_VERSION(DataLoader, 0)