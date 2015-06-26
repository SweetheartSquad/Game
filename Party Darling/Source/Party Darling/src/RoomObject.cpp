#include <Furniture.h>
#include <math.h>
#include <shader/ComponentShaderBase.h>

RoomObject::RoomObject(BulletWorld * _world, TriMesh * _mesh, ComponentShaderBase * _shader, bool _convex, Anchor_t _anchor):
	BulletMeshEntity(_world, _mesh),
	anchor(_anchor)
{
	setColliderAsMesh(_mesh, _convex);
	createRigidBody(25);
	if(_shader != nullptr){
		setShader(_shader, true);
	}
}

RoomObject::~RoomObject(void){
}
