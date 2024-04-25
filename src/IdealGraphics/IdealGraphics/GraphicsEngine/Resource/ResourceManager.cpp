#include "GraphicsEngine/Resource/ResourceManager.h"

using namespace Ideal;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

std::shared_ptr<Ideal::Model> ResourceManager::GetModel(const std::wstring& Key)
{
	std::shared_ptr<Ideal::Model> ret = m_models[Key];
	return ret;
}

void ResourceManager::AddModel(const std::wstring& Key, std::shared_ptr<Ideal::Model> Model)
{
	m_models[Key] = Model;
}
