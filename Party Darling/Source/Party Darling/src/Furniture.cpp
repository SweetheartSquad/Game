#pragma once

#include <Furniture.h>
#include <math.h>
#include <shader/ComponentShaderBase.h>

Furniture::Furniture(BulletWorld * _world, TriMesh * _mesh, ComponentShaderBase * _shader, Anchor_t _anchor):
	RoomObject(_world, _mesh, _shader, false, _anchor)
{
}

Furniture::~Furniture(void){
}
