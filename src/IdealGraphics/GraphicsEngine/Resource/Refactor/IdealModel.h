#pragma once
#include "Core/Core.h"

namespace Ideal
{
	class IdealMesh;
}

class D3DRenderer;

namespace Ideal
{
	class IdealModel
	{
	public:
		IdealModel();
		virtual ~IdealModel();

	public:


	private:
		std::vector<std::shared_ptr<Ideal::IdealMesh>> m_meshes;
	};
}