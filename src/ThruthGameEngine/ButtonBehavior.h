#pragma once
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include "TypeInfoMacros.h"
#include <string>
#include <memory>
namespace Truth
{
	class Managers;
	class UI;
	class Entity;
	class TextUI;
}
namespace Truth
{
	class ButtonBehavior
	{
		GENERATE_CLASS_TYPE_INFO(ButtonBehavior);

	private:
		friend class boost::serialization::access;
		friend class UI;

		BOOST_SERIALIZATION_SPLIT_MEMBER();

		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	protected:
		Managers* m_managers;
		UI* m_UI;
		TextUI* m_TextUI;
		Entity* m_owner;

	public:
		std::string m_name;

	public:
		ButtonBehavior();
		virtual ~ButtonBehavior();

	public:
		virtual void Initialize(Managers* _managers, UI* _UI, Entity* _owner);
		virtual void Initialize(Managers* _managers, TextUI* _UI, Entity* _owner);
		virtual void OnMouseClick() {};
		virtual void OnMouseUp() {};
		virtual void OnMouseOver() {};
		virtual void Update() {};
		virtual void Awake() {};
		virtual void Start() {};
	};

	template<class Archive>
	void Truth::ButtonBehavior::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& m_name;
	}

	template<class Archive>
	void Truth::ButtonBehavior::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& m_name;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::ButtonBehavior)
BOOST_CLASS_VERSION(Truth::ButtonBehavior, 0)