#pragma once
#include "Component.h"
namespace Truth
{
    class Button :
        public Component
    {
		GENERATE_CLASS_TYPE_INFO(Button);

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

		enum class BUTTON_STATE
		{
			IDEL,
			OVER,
			DOWN,
			END,
		};

	private:
		std::shared_ptr<Ideal::ISprite> m_sprite[static_cast<uint32>(BUTTON_STATE::END)];

		PROPERTY(texturePath);
		std::wstring m_texturePath[static_cast<uint32>(BUTTON_STATE::END)];

		PROPERTY(scale);
		Vector2 m_scale;
		PROPERTY(position);
		Vector2 m_position;
		PROPERTY(isActive);
		bool m_isActive;
		PROPERTYM(alpha, 0.0f, 1.0f);
		float m_alpha;
		PROPERTYM(zDepth, 0.0f, 1.0f);
		float m_zDepth;
		
		uint32 m_buttonState;

		Method* m_clickFunc;

	public:
		Button();
		virtual ~Button();


#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
    };
	template<class Archive>
	void Truth::Button::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}

	template<class Archive>
	void Truth::Button::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::Button)
BOOST_CLASS_VERSION(Truth::Button, 0)



