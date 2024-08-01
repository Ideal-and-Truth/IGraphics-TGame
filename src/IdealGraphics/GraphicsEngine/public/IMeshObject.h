#pragma once
#include "IRendererResource.h"
#include "../Utils/SimpleMath.h"
#include <memory>

namespace Ideal
{
	class IMesh;
}

namespace Ideal
{
	enum EMeshType
	{
		Static,
		Skinned
	};

	class IMeshObject : public IRendererResource
	{
	public:
		IMeshObject() {}
		virtual ~IMeshObject() {}

	public:
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Matrix) abstract;
		virtual void SetDrawObject(bool IsDraw) abstract;

		virtual Ideal::EMeshType GetMeshType() const abstract;

	public:
		virtual unsigned int GetMeshesSize() abstract;
		virtual std::shared_ptr<Ideal::IMesh> GetMeshByIndex(unsigned int) abstract;

	};
}