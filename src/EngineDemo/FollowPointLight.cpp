#include "FollowPointLight.h"

BOOST_CLASS_EXPORT_IMPLEMENT(FollowPointLight)

FollowPointLight::FollowPointLight()
{
}

FollowPointLight::~FollowPointLight()
{

}

void FollowPointLight::EditorSetValue()
{
	PointLight::EditorSetValue();
	SetPosition(m_owner.lock()->GetWorldPosition());
}

void FollowPointLight::Update()
{
	SetPosition(m_owner.lock()->GetWorldPosition());
}
