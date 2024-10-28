#include "CineCamera.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::CineCamera)

Truth::CineCamera::CineCamera()
	: m_isMove(false)
{

}

Truth::CineCamera::~CineCamera()
{

}

void Truth::CineCamera::Update()
{
	if (!m_isMove)
		return;

	
}
