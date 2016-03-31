#pragma once

#include <PD_InteractionRayCallback.h>
#include <PD_Masks.h>

PD_InteractionRayCallback::PD_InteractionRayCallback(const btVector3 & rayFromWorld, const btVector3 & rayToWorld, btCollisionObject* pObjectToIgnore)
	: btCollisionWorld::AllHitsRayResultCallback(rayFromWorld, rayToWorld), m_collisionObjectToIgnore(pObjectToIgnore)
{
	m_collisionFilterMask = -1;//kPD_INTERACTIVE | btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;
}

btScalar PD_InteractionRayCallback::AddSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace){
    if (rayResult.m_collisionObject != m_collisionObjectToIgnore){
        return btCollisionWorld::AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
    }
    return rayResult.m_hitFraction;
}