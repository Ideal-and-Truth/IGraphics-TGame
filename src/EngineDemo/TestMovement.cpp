#include "TestMovement.h"

TestMovement::TestMovement()
{

}

TestMovement::~TestMovement()
{

}

void TestMovement::Update()
{
	Vector3 movement(0.0f, -1.0f, 0.0f);
	Translate(movement);
}
