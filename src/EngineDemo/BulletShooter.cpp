#include "BulletShooter.h"
#include "Shooter.h"

BulletShooter::BulletShooter()
{
	m_name = "BulletShooter";
	m_layer = 1;
}

BulletShooter::~BulletShooter()
{

}

void BulletShooter::Initailize()
{
	__super::Initailize();
	AddComponent<Shooter>();
}
