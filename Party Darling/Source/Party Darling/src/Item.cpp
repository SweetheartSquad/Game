#pragma once

#include <Item.h>
#include <shader/ComponentShaderBase.h>

Item::Item(BulletWorld * _world, TriMesh * _mesh, ComponentShaderBase * _shader):
	RoomObject(_world, _mesh, _shader, Anchor_t::GROUND)
{
	setColliderAsBoundingBox();
	createRigidBody(25);
}

Item::~Item(void){
}
