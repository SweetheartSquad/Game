#pragma once

#include <Item.h>

<<<<<<< HEAD
Item::Item(BulletWorld * _world, TriMesh * _mesh, ComponentShaderBase * _shader):
	RoomObject(_world, _mesh, _shader, Anchor_t::GROUND)
=======
Item::Item(BulletWorld * _world, MeshInterface * _mesh):
	RoomObject(_world, _mesh, Anchor_t::GROUND)
>>>>>>> origin/E1-FurniturePlacement
{
	setColliderAsBoundingBox();
	createRigidBody(25);
}

Item::~Item(void){
}
