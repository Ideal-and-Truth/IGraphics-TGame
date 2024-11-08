#include "AnchorCamera.h"
#include "Managers.h"
#include "GraphicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(AnchorCamera)

AnchorCamera::AnchorCamera()
	: m_anchor{}
	, m_speed(1.0f)
	, m_distance(1.0f)
	, m_azimuth(0.0f)
{

}

AnchorCamera::~AnchorCamera()
{
	
}

void AnchorCamera::Awake()
{
	m_managers.lock()->Graphics()->SetMainCamera(this);
	m_position = m_anchor + Vector3{ m_distance, 0.0f, 0.0f };
	(m_position - m_anchor).Normalize(m_look);
}

void AnchorCamera::Update()
{
	float speed = m_speed * GetDeltaTime();

	m_azimuth += speed;

	// 카메라 위치 계산
	m_position.x = m_anchor.x + m_distance * cosf(m_azimuth);
	m_position.z = m_anchor.z + m_distance * sinf(m_azimuth);
	m_position.y = m_height;

	m_owner.lock()->SetPosition(m_position);

	(m_anchor - m_position).Normalize(m_look);
}
