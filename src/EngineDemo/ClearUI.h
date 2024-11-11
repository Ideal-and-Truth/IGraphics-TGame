#pragma once
#include "ButtonBehavior.h"

class Enemy;

class ClearUI
	: public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(ClearUI);

private:
	friend class boost::serialization::access;
	friend class UI;

	BOOST_SERIALIZATION_SPLIT_MEMBER();

	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

	std::weak_ptr<Enemy> m_boss;

	float m_speed;

	float m_alpha;

	bool m_isShown;

	bool m_revers = false;

public:
	ClearUI();
	virtual ~ClearUI();

private:
	virtual void Start() override;
	virtual void Update() override;

	void Trigger([[maybe_unused]] const void* _);

};

template<class Archive>
void ClearUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void ClearUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

BOOST_CLASS_EXPORT_KEY(ClearUI)
BOOST_CLASS_VERSION(ClearUI, 0)