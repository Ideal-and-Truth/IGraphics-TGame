#pragma once
#include "Component.h"
namespace Truth
{
	class PointLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(PointLight);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

		std::shared_ptr<Ideal::IPointLight> m_pointLight;
	public:

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(position);
		Vector3 m_position;

		PROPERTY(radius);
		float m_radius;

		PROPERTY(intensity);
		float m_intensity;

		PROPERTY(lightColor);
		Color m_lightColor;

	public:
		PointLight();
		virtual ~PointLight();

		void SetLight();
		void SetIntensity();
		void SetColor();
		void SetRange();

		METHOD(Initialize);
		void Initialize();

		METHOD(ApplyTransform);
		void ApplyTransform();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};
	template<class Archive>
	void Truth::PointLight::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_position;
		_ar& m_radius;
		_ar& m_intensity;
		_ar& m_lightColor;
	}

	template<class Archive>
	void Truth::PointLight::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_position;
		_ar& m_radius;
		_ar& m_intensity;
		_ar& m_lightColor;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::PointLight)
BOOST_CLASS_VERSION(Truth::PointLight, 0)