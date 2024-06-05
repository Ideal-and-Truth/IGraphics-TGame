#include "DefaultCamera.h"
#include "Camera.h"

Truth::DefaultCamera::DefaultCamera()
{
	m_name = "DefaultCamera";
}

Truth::DefaultCamera::~DefaultCamera()
{

}

void Truth::DefaultCamera::Initailize()
{
	__super::Initailize();
	auto c = AddComponent<Truth::Camera>();
	c->SetMainCamera();
}
