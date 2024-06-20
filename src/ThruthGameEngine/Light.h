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
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

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
		void AddLight(std::shared_ptr<Ideal::ILight> _light);

		METHOD(Initalize);
		void Initalize();

		METHOD(ApplyTransform);
		void ApplyTransform();

#ifdef _DEBUG
		virtual void EditorSetValue();
#endif // _DEBUG

	};

	template<class Archive>
	void Truth::Light::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}

}

BOOST_CLASS_EXPORT_KEY(Truth::Light)