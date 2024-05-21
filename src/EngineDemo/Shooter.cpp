#include "Shooter.h"
#include "Bullet.h"

Shooter::Shooter()
{

}

Shooter::~Shooter()
{

}

void Shooter::Awake()
{
}

void Shooter::Update()
{
	if (GetKeyDown(KEY::SPACE))
	{
		AddEntity<Bullet>();
	}
}
