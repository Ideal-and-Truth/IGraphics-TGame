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

	private:
		std::shared_ptr<Ideal::ISprite> m_sprite;

		PROPERTY(texturePath);
		std::wstring m_texturePath;
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

		METHOD(Awake);
		virtual void Awake() override;

		METHOD(Update);
		virtual void Update() override;

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};


	template<class Archive>
	void Truth::UI::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_texturePath;
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
		_ar& m_texturePath;
		_ar& m_scale;
		_ar& m_position;
		_ar& m_isActive;
		_ar& m_alpha;
		_ar& m_zDepth;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::UI)
BOOST_CLASS_VERSION(Truth::UI, 0)

