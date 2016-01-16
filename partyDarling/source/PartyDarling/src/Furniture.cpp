#pragma once

#include <Furniture.h>

Furniture::Furniture(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor):
	RoomObject(_world, _mesh, _anchor)
{
	setColliderAsBoundingBox();
	createRigidBody(0);
}

Furniture::~Furniture(void){
}