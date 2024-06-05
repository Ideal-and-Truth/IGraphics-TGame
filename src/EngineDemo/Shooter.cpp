#include "Shooter.h"
#include "Bullet.h"

Shooter::Shooter()
{
	m_name = "Shooter";
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
