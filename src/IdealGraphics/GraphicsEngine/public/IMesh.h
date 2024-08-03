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
		virtual void SetMaterialObject(std::shared_ptr<Ideal::IMaterial> Material) abstract;

		// ���߿� �̰� ���ٵ�
		virtual std::shared_ptr<Ideal::IMaterial> GetMaterialObject() abstract;
	};
}