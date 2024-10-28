#pragma once
#include "Component.h"

namespace Truth
{
	class Camera;
}

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
			Vector3	m_look;
			CINE_CAMERA_MOVE_MODE m_moveMode;
			CINE_CAMERA_ROTATION_MODE m_rotaionMode;
			float m_delayTime;

			std::vector<Vector3> m_curvePositionPoint;
			std::vector<Vector3> m_curveRotationPoint;
		};

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::vector<CameraNode> m_node;

		std::weak_ptr<Camera> m_mainCamera;

		bool m_isMove;

		bool m_isEnd;

		uint32 m_currentNode;
		uint32 m_nextNode;

		float m_dt;

		PROPERTY(dataPath);
		std::string m_dataPath;

	public:
		CineCamera();
		virtual ~CineCamera();

		void Play();

		void LoadData(const fs::path& _dataPath);

	private:
		METHOD(Update);
		void Update();

		METHOD(Initialize);
		void Initialize();
	};
}

template<class Archive>
void Truth::CineCamera::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);
	_ar& m_dataPath;
}

template<class Archive>
void Truth::CineCamera::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
	_ar& m_dataPath;
}
BOOST_CLASS_EXPORT_KEY(Truth::CineCamera)
BOOST_CLASS_VERSION(Truth::CineCamera, 0)