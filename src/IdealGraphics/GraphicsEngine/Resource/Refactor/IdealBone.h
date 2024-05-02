#pragma once
#include "Core/Core.h"

namespace Ideal
{
	class IdealBone
	{
	public:
		IdealBone();
		virtual ~IdealBone() {};

		void SetName(const std::string& Name) { m_boneName = Name; }
		const std::string& GetName() const { return m_boneName; }

		void SetBoneIndex(const int32& Index) { m_boneIndex = Index; }
		int32 GetBoneIndex() const { return m_boneIndex; }

		void SetParent(const int32& Index) { m_parent = Index; }
		int32 GetParent() { return m_parent; }

		const Matrix& GetTransform() const { return m_transform; }
		void SetTransform(Matrix Transform) { m_transform = Transform; }

	private:
		std::string m_boneName;
		int32 m_boneIndex;
		int32 m_parent;
		Matrix m_transform;
	};
}