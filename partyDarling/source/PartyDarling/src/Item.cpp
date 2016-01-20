#pragma once

#include <Item.h>

Item::Item(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader) :
	RoomObject(_world, _mesh, _shader, Anchor_t::GROUND)
{
	setColliderAsBoundingBox();
	createRigidBody(25);
}