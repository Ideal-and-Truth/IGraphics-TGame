#include "DataLoader.h"
BOOST_CLASS_EXPORT_IMPLEMENT(DataLoader)

DataLoader::DataLoader()
{
	m_name = "DataLoader";
}

DataLoader::~DataLoader()
{

}

void DataLoader::Awake()
{
	EventPublish("LoadData", nullptr);
}
