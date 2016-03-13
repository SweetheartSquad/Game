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
	billboarded(false),
	parentDependent(false),
	parentMax(-1),
	padding(0)
{
#ifdef _DEBUG
	 QuadMesh * m = new QuadMesh(true, GL_LINE_STRIP);
	 float _halfSize = 0.5f;
	//Top
	m->pushVert(Vertex(-_halfSize, _halfSize, _halfSize));
	m->pushVert(Vertex(-_halfSize, _halfSize, -_halfSize));
	m->pushVert(Vertex(_halfSize, _halfSize, -_halfSize));
	m->pushVert(Vertex(_halfSize, _halfSize, _halfSize));
	m->setNormal(0, 0.0, 1.0, 0.0);
	m->setNormal(1, 0.0, 1.0, 0.0);
	m->setNormal(2, 0.0, 1.0, 0.0);
	m->setNormal(3, 0.0, 1.0, 0.0);
	m->setUV(0, 0.0, 0.0);
	m->setUV(1, 0.0, 1.0);
	m->setUV(2, 1.0, 1.0);
	m->setUV(3, 1.0, 0.0);

	//Bottom
	m->pushVert(Vertex(-_halfSize, -_halfSize, _halfSize));
	m->pushVert(Vertex(_halfSize, -_halfSize, _halfSize));
	m->pushVert(Vertex(_halfSize, -_halfSize, -_halfSize));
	m->pushVert(Vertex(-_halfSize, -_halfSize, -_halfSize));
	m->setNormal(4, 0.0, -1.0, 0.0);
	m->setNormal(5, 0.0, -1.0, 0.0);
	m->setNormal(6, 0.0, -1.0, 0.0);
	m->setNormal(7, 0.0, -1.0, 0.0);
	m->setUV(4, 0.0, 0.0);
	m->setUV(5, 0.0, 1.0);
	m->setUV(6, 1.0, 1.0);
	m->setUV(7, 1.0, 0.0);

	//Front
	m->pushVert(Vertex(-_halfSize, _halfSize, _halfSize));
	m->pushVert(Vertex(_halfSize, _halfSize, _halfSize));
	m->pushVert(Vertex(_halfSize, -_halfSize, _halfSize));
	m->pushVert(Vertex(-_halfSize, -_halfSize, _halfSize));
	m->setNormal(8, 0.0, 0.0, 1.0);
	m->setNormal(9, 0.0, 0.0, 1.0);
	m->setNormal(10, 0.0, 0.0, 1.0);
	m->setNormal(11, 0.0, 0.0, 1.0);
	m->setUV(8, 0.0, 0.0);
	m->setUV(9, 0.0, 1.0);
	m->setUV(10, 1.0, 1.0);
	m->setUV(11, 1.0, 0.0);

	//Back
	m->pushVert(Vertex(_halfSize, _halfSize, -_halfSize));
	m->pushVert(Vertex(-_halfSize, _halfSize, -_halfSize));
	m->pushVert(Vertex(-_halfSize, -_halfSize, -_halfSize));
	m->pushVert(Vertex(_halfSize, -_halfSize, -_halfSize));
	m->setNormal(12, 0.0, 0.0, -1.0);
	m->setNormal(13, 0.0, 0.0, -1.0);
	m->setNormal(14, 0.0, 0.0, -1.0);
	m->setNormal(15, 0.0, 0.0, -1.0);
	m->setUV(12, 0.0, 0.0);
	m->setUV(13, 0.0, 1.0);
	m->setUV(14, 1.0, 1.0);
	m->setUV(15, 1.0, 0.0);

	//Left
	m->pushVert(Vertex(-_halfSize, _halfSize, -_halfSize));
	m->pushVert(Vertex(-_halfSize, _halfSize, _halfSize));
	m->pushVert(Vertex(-_halfSize, -_halfSize, _halfSize));
	m->pushVert(Vertex(-_halfSize, -_halfSize, -_halfSize));
	m->setNormal(16, -1.0, 0.0, 0.0);
	m->setNormal(17, -1.0, 0.0, 0.0);
	m->setNormal(18, -1.0, 0.0, 0.0);
	m->setNormal(19, -1.0, 0.0, 0.0);
	m->setUV(16, 0.0, 0.0);
	m->setUV(17, 0.0, 1.0);
	m->setUV(18, 1.0, 1.0);
	m->setUV(19, 1.0, 0.0);

	//Right
	m->pushVert(Vertex(_halfSize, _halfSize, _halfSize));
	m->pushVert(Vertex(_halfSize, _halfSize, -_halfSize));
	m->pushVert(Vertex(_halfSize, -_halfSize, -_halfSize));
	m->pushVert(Vertex(_halfSize, -_halfSize, _halfSize));
	m->setNormal(20, 1.0, 0.0, 0.0);
	m->setNormal(21, 1.0, 0.0, 0.0);
	m->setNormal(22, 1.0, 0.0, 0.0);
	m->setNormal(23, 1.0, 0.0, 0.0);
	m->setUV(20, 0.0, 0.0);
	m->setUV(21, 0.0, 1.0);
	m->setUV(22, 1.0, 1.0);
	m->setUV(23, 1.0, 0.0);

	boundingBoxMesh = new MeshEntity(m, _shader);
	childTransform->addChild(boundingBoxMesh);
#endif
}

RoomObject::~RoomObject(void){
}

void RoomObject::resetObject(){
	glm::quat orient = childTransform->getOrientationQuat();
	float a = glm::angle(orient);
	glm::vec3 axis = glm::axis(orient);
	rotatePhysical(-a, axis.x, axis.y, axis.z);

	translatePhysical(originalPos, false);

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
		rotatePhysical(angle, 0, 1, 0, false);
	}
}

void RoomObject::moveChildren(glm::vec3 _translation, bool _relative){
	typedef std::map<PD_Side, PD_Slot *>::iterator it_type;
	for(it_type iterator = emptySlots.begin(); iterator != emptySlots.end(); iterator++) {
		for(auto c : iterator->second->children){
			c->translatePhysical(_translation, _relative);
			c->moveChildren(_translation, _relative);
			c->realign();
			c->meshTransform->makeCumulativeModelMatrixDirty();
		}
	}
}