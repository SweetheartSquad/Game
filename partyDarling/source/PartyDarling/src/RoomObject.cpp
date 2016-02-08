#include <RoomObject.h>
#include <MeshInterface.h>
#include <Box.h>

Slot::Slot(PD_Side _childSide, float _loc, float _length) :
	childSide(_childSide),
	loc(_loc),
	length(_length)
{
}

RoomObject::RoomObject(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader, Anchor_t _anchor):
	BulletMeshEntity(_world, _mesh, _shader),
	anchor(_anchor),
	boundingBox(mesh->calcBoundingBox()),
	parent(nullptr),
	type("")
{
	 
}

RoomObject::~RoomObject(void){
}