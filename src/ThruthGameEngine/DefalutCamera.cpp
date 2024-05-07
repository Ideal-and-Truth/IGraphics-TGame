#include "DefalutCamera.h"
#include "Camera.h"

Truth::DefalutCamera::DefalutCamera()
{

}

Truth::DefalutCamera::~DefalutCamera()
{

}

void Truth::DefalutCamera::Initailize()
{
	 auto c = AddComponent<Truth::Camera>();
	 c->SetMainCamera();
}
