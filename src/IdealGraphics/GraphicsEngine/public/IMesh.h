#pragma once
#include <memory>
#include <string>

namespace Ideal
{
	class IMaterial;
}

namespace Ideal
{
	class IMesh
	{
	public:
		IMesh() {}
		virtual ~IMesh() {}

	public:
		virtual std::string GetName() abstract;
		virtual void SetMaterial(std::shared_ptr<Ideal::IMaterial> Material) abstract;
		virtual std::shared_ptr<Ideal::IMaterial> GetMaterialObject() abstract;
	};
}