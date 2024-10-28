#pragma once
#include "Component.h"

namespace Truth
{
	enum class CINE_CAMERA_MOVE_MODE
	{
		DIRECT,
		LERP,
		CURVE
	};

	enum class CINE_CAMERA_ROTATION_MODE
	{
		DIRECT,
		LERP,
		CURVE
	};

	class CineCamera :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(CineCamera);

		struct CameraNode
		{
			Vector3 m_position;
			Quaternion m_look;
			CINE_CAMERA_MOVE_MODE m_moveMode;
			CINE_CAMERA_ROTATION_MODE m_rotaionMode;
			float m_delayTime;
		};

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	public:
		CineCamera();
		virtual ~CineCamera();

	private:
		std::vector<CameraNode> m_node;

		bool m_isMove;

		uint32 m_currentNode;
		uint32 m_nextNode;

		float m_dt;

		void Update();
	};
}

template<class Archive>
void Truth::CineCamera::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);
}

template<class Archive>
void Truth::CineCamera::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}
BOOST_CLASS_EXPORT_KEY(Truth::CineCamera)
BOOST_CLASS_VERSION(Truth::CineCamera, 0)