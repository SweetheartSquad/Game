#include <RoomObject.h>

RoomObject::RoomObject(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor):
	BulletMeshEntity(_world, _mesh),
	anchor(_anchor)
{
}

RoomObject::~RoomObject(void){
}
