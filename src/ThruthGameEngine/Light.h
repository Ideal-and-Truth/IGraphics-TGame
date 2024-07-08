#pragma once
#include "Component.h"
namespace Truth
{
	enum class LightType
	{
		Directional,
		Spot,
		Point,
	};

	class Light :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Light);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		LightType m_lightType;
		std::shared_ptr<Ideal::IDirectionalLight> m_directionalLight;
		std::shared_ptr<Ideal::ISpotLight> m_spotLight;
		std::shared_ptr<Ideal::IPointLight> m_pointLight;

		PROPERTY(direction);
		Vector3 m_direction;

		PROPERTY(position);
		Vector3 m_position;

		PROPERTY(isRendering);
		bool m_isRendering;

	public:
		Light();
		virtual ~Light();

		void SetLight(LightType InLightType);

		METHOD(Initalize);
		void Initalize();

		METHOD(ApplyTransform);
		void ApplyTransform();

#ifdef _DEBUG
		virtual void EditorSetValue();
#endif // _DEBUG

	};

	template<class Archive>
	void Truth::Light::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}

	template<class Archive>
	void Truth::Light::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::Light)
BOOST_CLASS_VERSION(Truth::Light, 0)