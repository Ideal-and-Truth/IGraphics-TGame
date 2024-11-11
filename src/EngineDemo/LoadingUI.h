#pragma once
#include "Component.h"

namespace Ideal
{
	class ISprite;
}

class LoadingUI :
    public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(LoadingUI);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

	PROPERTY(path);
	std::string m_path;

	PROPERTY(nextMap);
	std::string m_nextMap;

	std::shared_ptr<Ideal::ISprite> m_sprite;

	bool m_isActive = false;

	float m_speed = 0.5f;
	float m_alpha = 0.0f;

public:
	LoadingUI();
	virtual ~LoadingUI();

private:
	METHOD(Start);
	virtual void Start() override;

	METHOD(Update);
	virtual void Update() override;

	METHOD(Destroy);
	virtual void Destroy() override;

	void Active(const void* _);

	
};

template<class Archive>
void LoadingUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);
	_ar& m_path;
	_ar& m_nextMap;
}

template<class Archive>
void LoadingUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
	_ar& m_path;
	_ar& m_nextMap;
}


BOOST_CLASS_EXPORT_KEY(LoadingUI)
BOOST_CLASS_VERSION(LoadingUI, 0)