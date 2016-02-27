#include <RoomObject.h>
#include <MeshInterface.h>
#include <Box.h>
#include <PD_Slot.h>

RoomObject::RoomObject(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader, Anchor_t _anchor):
	BulletMeshEntity(_world, _mesh, _shader),
	anchor(_anchor),
	boundingBox(mesh->calcBoundingBox()),
	parent(nullptr),
	type(""),
	angle(0)
{
	 
}

RoomObject::~RoomObject(void){
}

void RoomObject::resetObject(){
	glm::quat orient = childTransform->getOrientationQuat();
	float a = glm::angle(orient);
	glm::vec3 axis = glm::axis(orient);
	rotatePhysical(-a, axis.x, axis.y, axis.z);

	translatePhysical(glm::vec3(), false);

	typedef std::map<PD_Side, PD_Slot *>::iterator it_type;
	for(it_type iterator = emptySlots.begin(); iterator != emptySlots.end(); iterator++) {
		PD_Slot * slot = iterator->second;
		slot->spaceFilled = 0;
		slot->children.clear();
	}
	parent = nullptr;
}