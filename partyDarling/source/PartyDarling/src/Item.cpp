#pragma once

#include <Item.h>

Item::Item(BulletWorld * _world, MeshInterface * _mesh) :
	RoomObject(_world, _mesh, Anchor_t::GROUND)
{
	setColliderAsBoundingBox();
	createRigidBody(25);
}