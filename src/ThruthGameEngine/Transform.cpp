#include "Transform.h"

Transform::Transform()
	: m_position(0.0f, 0.0f, 0.0f)
	, m_scale(1.0f, 1.0f, 1.0f)
	, m_rotation{}
{
	m_canMultiple = false;
	SetName("TransForm");
}

Transform::~Transform()
{

}

void Transform::Update(float4 _dt)
{
}

void Transform::Render()
{
}

void Transform::LateUpdate(float4 _dt)
{
}

void Transform::FiexUpdate(float4 _dt)
{
}
