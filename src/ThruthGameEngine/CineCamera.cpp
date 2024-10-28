#include "CineCamera.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::CineCamera)

Truth::CineCamera::CineCamera()
	: m_isMove(true)
	, m_currentNode(0)
	, m_nextNode(1)
	, m_dt(0)
	, m_isEnd(false)
{

}

Truth::CineCamera::~CineCamera()
{

}

void Truth::CineCamera::Update()
{
	if (!m_isMove)
		return;

	CameraNode& current = m_node[m_currentNode];
	CameraNode& next = m_node[m_nextNode];

	while (current.m_delayTime >= m_dt)
	{
		if (m_currentNode >= m_node.size() - 1)
		{
			m_isEnd = true;
			m_isMove = false;
			m_currentNode = 0;
			m_nextNode = 1;
			return;
		}
		m_dt -= current.m_delayTime;
		m_currentNode = m_nextNode;
		m_nextNode++;
	}

	switch (current.m_moveMode)
	{
	case CINE_CAMERA_MOVE_MODE::DIRECT:

	default:
		break;
	}
}
