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
	EventBind("Update", &Shooter::Update);
}

void Shooter::Update(std::any _p)
{
	if (GetKeyDown(KEY::SPACE))
	{
		AddEntity<Bullet>();
	}
}
