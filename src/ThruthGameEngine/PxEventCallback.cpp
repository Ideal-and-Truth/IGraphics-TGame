#include "PxEventCallback.h"
#include "Collider.h"
#include "Entity.h"

Truth::PxEventCallback::PxEventCallback()
{
}

Truth::PxEventCallback::~PxEventCallback()
{
}

void Truth::PxEventCallback::onContact(const physx::PxContactPairHeader& _pairHeader, const physx::PxContactPair* _pairs, physx::PxU32 _nbPairs)
{
	for (uint32 i = 0; i < _nbPairs; i++)
	{
		const physx::PxContactPair& contactPair = _pairs[i];

		const Collider* a = static_cast<Collider*>(contactPair.shapes[0]->userData);
		const Collider* b = static_cast<Collider*>(contactPair.shapes[1]->userData);
		if (a)
		{
			a->GetOwner().lock()->OnCollisionEnter(b);
		}
		if (b)
		{
			b->GetOwner().lock()->OnCollisionEnter(a);
		}
	}
}


void Truth::PxEventCallback::onTrigger(physx::PxTriggerPair* _pairs, physx::PxU32 _count)
{
}

void Truth::PxEventCallback::onWake(physx::PxActor** _actors, physx::PxU32 _count)
{
}

void Truth::PxEventCallback::onSleep(physx::PxActor** _actors, physx::PxU32 _count)
{
}

void Truth::PxEventCallback::onAdvance(const physx::PxRigidBody* const* _bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 _count)
{
}

void Truth::PxEventCallback::onConstraintBreak(physx::PxConstraintInfo* _constraints, physx::PxU32 _count)
{
}
