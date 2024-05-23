#include "BulletShooter.h"
#include "Shooter.h"

BulletShooter::BulletShooter()
{

}

BulletShooter::~BulletShooter()
{

}

void BulletShooter::Initailize()
{
	__super::Initailize();
	AddComponent<Shooter>();
}
