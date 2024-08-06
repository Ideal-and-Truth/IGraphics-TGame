#pragma once
#include "Core/Core.h"

namespace Ideal
{
	class ResourceBase
	{
	public:
		ResourceBase();
		virtual ~ResourceBase();

	public:
		void SetName(const std::string& Name);
		const std::string& GetName();

		uint64 GetID();
		void SetID(uint64 ID);

	protected:
		std::string m_name;
		uint64 m_id;
	};
}