#include <RoomObject.h>
#include <shader/ComponentShaderBase.h>

RoomObject::RoomObject(BulletWorld * _world, MeshInterface * _mesh, ComponentShaderBase * _shader, bool _convex, Anchor_t _anchor):
	BulletMeshEntity(_world, _mesh),
	anchor(_anchor)
{
	
	if(_shader != nullptr){
		setShader(_shader, true);
	}
}

RoomObject::~RoomObject(void){
}
