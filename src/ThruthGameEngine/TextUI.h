#pragma once
#include "Component.h"
#include "UISpriteSet.h"

namespace Ideal
{
	class IText;
}

namespace Truth
{
	class ButtonBehavior;
}

namespace Truth
{
	class TextUI :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(TextUI);

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();

		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<Ideal::IText> m_textSprite;

		PROPERTY(size);
		Vector2 m_size;

		PROPERTY(position);
		Vector2 m_position;

		PROPERTYM(alpha, 0.0f, 1.0f);
		float m_alpha;

		PROPERTYM(zDepth, 0.0f, 0.9f);
		float m_zDepth;

		PROPERTY(behavior);
		std::shared_ptr<ButtonBehavior> m_behavior;

		PROPERTY(text);
		std::wstring m_text;

		PROPERTY(fontSize);
		float m_fontSize;

	public:
		TextUI();
		virtual ~TextUI();

		void ChangeText(const std::wstring& _text);

	private:
		METHOD(Initialize);
		virtual void Initialize() override;

		METHOD(Start);
		virtual void Start() override;

		METHOD(Awake);
		virtual void Awake() override;

		METHOD(Update);
		virtual void Update() override;


		bool IsActive();

		void SetSpriteActive(bool _active);

		METHOD(ResizeWindow);
		void ResizeWindow() override;

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};


	template<class Archive>
	void Truth::TextUI::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_size;
		_ar& m_text;
		_ar& m_position;
		_ar& m_alpha;
		_ar& m_zDepth;
		_ar& m_behavior;
		_ar& m_fontSize;
	}

	template<class Archive>
	void Truth::TextUI::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_size;
		_ar& m_text;
		_ar& m_position;
		_ar& m_alpha;
		_ar& m_zDepth;
		_ar& m_behavior;
		_ar& m_fontSize;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::TextUI)
BOOST_CLASS_VERSION(Truth::TextUI, 0)

