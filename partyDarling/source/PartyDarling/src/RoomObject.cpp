#include <RoomObject.h>
#include <MeshInterface.h>
#include <Box.h>
#include <PD_Slot.h>

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