#pragma once
#include "Core/Core.h"

namespace Ideal
{
	class Mesh;
	class Model;
}
namespace Ideal
{
	class ResourceManager
	{
	public:
		ResourceManager();
		virtual ~ResourceManager();

	public:
		std::shared_ptr<Ideal::Model> GetModel(const std::wstring& Key);
		void AddModel(const std::wstring& Key, std::shared_ptr<Ideal::Model> Model);

	private:
		std::unordered_map<std::wstring, std::shared_ptr<Ideal::Model>> m_models;
	};
}