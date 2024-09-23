#pragma once
#include "IRendererResource.h"
#include "../Utils/SimpleMath.h"
#include <memory>

namespace Ideal
{
	class IMesh;
	class IBone;
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

		virtual DirectX::SimpleMath::Matrix GetLocalTransformMatrix() abstract;

		virtual Ideal::EMeshType GetMeshType() const abstract;

		// 디버깅 매쉬일때만 작동, 스키닝 매쉬는 작동 안함
		virtual void SetDebugMeshColor(DirectX::SimpleMath::Color& Color) abstract;

	public:
		virtual unsigned int GetMeshesSize() abstract;
		virtual std::weak_ptr<Ideal::IMesh> GetMeshByIndex(unsigned int) abstract;
		
		virtual unsigned int GetBonesSize() abstract;
		virtual std::weak_ptr<Ideal::IBone> GetBoneByIndex(unsigned int) abstract;
	};
}