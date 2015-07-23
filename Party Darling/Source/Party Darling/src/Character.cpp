#pragma once

#include <Character.h>

Person::Person(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor):
	RoomObject(_world, _mesh, _anchor)
{
	setColliderAsBoundingBox();
	createRigidBody(25);
}

Person::~Person(void){
}
