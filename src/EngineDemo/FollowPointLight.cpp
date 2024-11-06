#include "FollowPointLight.h"

BOOST_CLASS_EXPORT_IMPLEMENT(FollowPointLight)

FollowPointLight::FollowPointLight()
{
}

FollowPointLight::~FollowPointLight()
{

}

#ifdef EDITOR_MODE
void FollowPointLight::EditorSetValue()
{
	PointLight::EditorSetValue();
	SetPosition(m_owner.lock()->GetWorldPosition());
}
#endif // EDITOR_MODE

void FollowPointLight::Update()
{
	SetPosition(m_owner.lock()->GetWorldPosition());
}
