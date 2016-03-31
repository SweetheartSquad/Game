#pragma once

#include <BulletCollision\CollisionDispatch\btCollisionWorld.h>

struct   PD_InteractionRayCallback : public btCollisionWorld::AllHitsRayResultCallback{
    PD_InteractionRayCallback(const btVector3 & rayFromWorld, const btVector3 & rayToWorld, btCollisionObject * pObjectToIgnore);

    btCollisionObject * m_collisionObjectToIgnore;

    virtual btScalar AddSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace);
};