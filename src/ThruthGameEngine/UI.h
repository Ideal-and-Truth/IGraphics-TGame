#pragma once
#include "Component.h"

namespace Ideal
{
	class ISprite;
}

namespace Truth
{
	class UI :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(UI);

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
		std::shared_ptr<Ideal::ISprite> m_sprite[3];

		fs::path m_texturePath[3];

		BUTTON_STATE m_state;

		PROPERTY(texture);
		std::shared_ptr<Texture> m_texture[3];

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

	public:
		UI();
		virtual ~UI();

	private:
		METHOD(Initalize);
		virtual void Initalize() override;

		METHOD(Update);
		virtual void Update() override;

		void OnMouseOver();
		void OnMouseClick();
		void OnMouseUp();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};


	template<class Archive>
	void Truth::UI::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);

		std::string texPath;
		_ar& texPath;
		m_texturePath = fs::path(texPath);

		_ar& m_scale;
		_ar& m_position;
		_ar& m_isActive;
		_ar& m_alpha;
		_ar& m_zDepth;
	}

	template<class Archive>
	void Truth::UI::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);

		_ar& m_texturePath.generic_string();
		_ar& m_scale;
		_ar& m_position;
		_ar& m_isActive;
		_ar& m_alpha;
		_ar& m_zDepth;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::UI)
BOOST_CLASS_VERSION(Truth::UI, 0)

