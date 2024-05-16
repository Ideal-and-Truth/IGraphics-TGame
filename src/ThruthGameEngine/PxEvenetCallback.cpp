#include "PxEvenetCallback.h"

Truth::PxEvenetCallback::PxEvenetCallback()
{
	int a = 1;
}

Truth::PxEvenetCallback::~PxEvenetCallback()
{
	int a = 1;
}

void Truth::PxEvenetCallback::onContact(const physx::PxContactPairHeader& _pairHeader, const physx::PxContactPair* _pairs, physx::PxU32 _nbPairs)
{
	PX_UNUSED((_pairHeader));
	std::vector<physx::PxContactPairPoint> contactPoints;

	for (physx::PxU32 i = 0; i < _nbPairs; i++)
	{
		physx::PxU32 contactCount = _pairs[i].contactCount;
		if (contactCount)
		{
			contactPoints.resize(contactCount);
			_pairs[i].extractContacts(&contactPoints[0], contactCount);

			for (physx::PxU32 j = 0; j < contactCount; j++)
			{
				int a = 1;
			}
		}
	}
}


void Truth::PxEvenetCallback::onTrigger(physx::PxTriggerPair* _pairs, physx::PxU32 _count)
{
	int a = 1;
}

void Truth::PxEvenetCallback::onWake(physx::PxActor** _actors, physx::PxU32 _count)
{
	int a = 1;
}

void Truth::PxEvenetCallback::onSleep(physx::PxActor** _actors, physx::PxU32 _count)
{
	int a = 1;
}

void Truth::PxEvenetCallback::onAdvance(const physx::PxRigidBody* const* _bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 _count)
{
	int a = 1;
}

void Truth::PxEvenetCallback::onConstraintBreak(physx::PxConstraintInfo* _constraints, physx::PxU32 _count)
{
	int a = 1;
}
