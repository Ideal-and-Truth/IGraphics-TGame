#pragma once
#include "Collider.h"
namespace Truth
{
	class BoxCollider :
		public Collider
	{
		GENERATE_CLASS_TYPE_INFO(BoxCollider)

	public:
		Vector3 m_size;

	public:
		BoxCollider(std::shared_ptr<Managers> _managers);
		virtual ~BoxCollider();

		virtual void Awake() override;
	};
}

