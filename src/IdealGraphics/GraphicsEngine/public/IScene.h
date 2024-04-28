#pragma once
#include "Core/Core.h"

namespace Ideal
{
	class IMeshObject;
}

namespace Ideal
{
	class IScene
	{
	public:
		IScene() {};
		virtual ~IScene() {};

	public:
		virtual void AddObject(std::shared_ptr<Ideal::IMeshObject> Mesh) abstract;

	};
}
