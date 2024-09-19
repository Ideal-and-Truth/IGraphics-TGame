#pragma once
#include "Component.h"

namespace Ideal
{
	class IMeshObject;
}

class BezierShooter :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(BezierShooter);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	PROPERTY(endPoint);
	Vector3 m_endPoint;

	PROPERTY(controlPoint);
	Vector3 m_controlPoint;

	Vector3 m_objPoint;

	std::vector<Vector3> m_points;

	PROPERTY(slope);
	float m_slope;

	float m_timer;

	std::vector<std::shared_ptr<Ideal::IMeshObject>> m_drawPoints;
	std::shared_ptr<Ideal::IMeshObject> m_obj;

public:
	BezierShooter();
	virtual ~BezierShooter();

	METHOD(Initalize);
	void Initalize();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();

	METHOD(ApplyTransform);
	void ApplyTransform();
};

template<class Archive>
void BezierShooter::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
	if (file_version == 1)
	{
		_ar& m_endPoint;
		_ar& m_controlPoint;
		_ar& m_slope;
	}

}

template<class Archive>
void BezierShooter::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);
	_ar& m_endPoint;
	_ar& m_controlPoint;
	_ar& m_slope;

}

BOOST_CLASS_EXPORT_KEY(BezierShooter)
BOOST_CLASS_VERSION(BezierShooter, 1)

