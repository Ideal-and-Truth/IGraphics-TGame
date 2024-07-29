#pragma once
#include "Component.h"
namespace Truth
{
	class SpotLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(SpotLight);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<Ideal::ISpotLight> m_spotLight;

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(position);
		Vector3 m_position;
		PROPERTY(direction);
		Vector4 m_direction;

		PROPERTY(angle);
		float m_angle;
		PROPERTY(range);
		float m_range;

		PROPERTY(intensity);
		float m_intensity;

		PROPERTY(lightColor);
		Color m_lightColor;

	public:
		SpotLight();
		virtual ~SpotLight();

		void SetLight();
		void SetIntensity();
		void SetColor();

		METHOD(Initalize);
		void Initalize();

		METHOD(ApplyTransform);
		void ApplyTransform();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};
	template<class Archive>
	void Truth::SpotLight::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}

	template<class Archive>
	void Truth::SpotLight::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}
}

BOOST_CLASS_EXPORT_KEY(Truth::SpotLight)
BOOST_CLASS_VERSION(Truth::SpotLight, 0)

