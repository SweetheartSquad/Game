#include <RoomObject.h>
#include <MeshInterface.h>
#include <Box.h>

Slot::Slot(float _loc, float _length) :
	loc(_loc),
	length(_length)
{
}

RoomObject::RoomObject(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader, Anchor_t _anchor):
	BulletMeshEntity(_world, _mesh, _shader),
	anchor(_anchor),
	boundingBox(mesh->calcBoundingBox()),
	parent(nullptr)
{
	 
}

RoomObject::~RoomObject(void){
}