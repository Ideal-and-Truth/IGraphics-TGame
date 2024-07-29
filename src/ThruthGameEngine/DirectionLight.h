#pragma once
#include "Component.h"
namespace Truth
{
	class DirectionLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(DirectionLight);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<Ideal::IDirectionalLight> m_directionalLight;

		PROPERTY(position);
		Vector3 m_position;

		PROPERTY(isRendering);
		bool m_isRendering;

	public:
		DirectionLight();
		virtual ~DirectionLight();

		void SetLight();

		METHOD(Initalize);
		void Initalize();

		METHOD(ApplyTransform);
		void ApplyTransform();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE

	};

	template<class Archive>
	void Truth::DirectionLight::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}

	template<class Archive>
	void Truth::DirectionLight::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::DirectionLight)
BOOST_CLASS_VERSION(Truth::DirectionLight, 0)