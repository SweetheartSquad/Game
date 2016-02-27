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
	angle(0),
	billboarded(false)
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

void RoomObject::billboard(glm::vec3 _playerPos){
	glm::vec3 cPos = getPhysicsBodyCenter();
	glm::vec3 d = glm::normalize(_playerPos - cPos);
		
	float a = glm::atan(d.x, d.z);
	a = glm::degrees(a);
		
	float angleDif = (a - angle);
	while(angleDif > 180){
		angleDif -= 360;
	}while(angleDif < -180){
		angleDif += 360;
	}
	if(glm::abs(angleDif) > FLT_EPSILON){
		angle += angleDif*0.05f;
		childTransform->setOrientation(glm::angleAxis(angle, glm::vec3(0,1,0)));
	}
}