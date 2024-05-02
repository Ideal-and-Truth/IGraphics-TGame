#pragma once
#include "Core/Core.h"

namespace Ideal
{
	class IMeshObject;
}

namespace Ideal
{
	class IRenderScene
	{
	public:
		IRenderScene() {};
		virtual ~IRenderScene() {};

	public:
		virtual void AddObject(std::shared_ptr<Ideal::IMeshObject> MeshObject) abstract;

	};
}
