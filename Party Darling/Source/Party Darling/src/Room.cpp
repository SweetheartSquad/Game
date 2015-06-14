#include <Room.h>
#include <math.h>
#include <shader/ComponentShaderBase.h>

Room::Room(BulletWorld * _world, ComponentShaderBase * _shader, RoomLayout_t _type, glm::vec2 _size, Texture * _wallTexture){

	// Create room boundaries (walls, floor, ceiling)
	std::vector<MeshInterface *> m = RoomLayout::getWalls(_type, _size);

	for(unsigned int i = 0; i < m.size(); ++i){
		boundaries.push_back(new BulletMeshEntity(_world, m.at(i)));
		childTransform->addChild(boundaries.at(i));
		boundaries.at(i)->setColliderAsBoundingBox();
		boundaries.at(i)->createRigidBody(0);
		boundaries.at(i)->mesh->pushTexture2D(_wallTexture);
	}

}

void Room::setShader(Shader * _shader, bool _default){
	for(unsigned int i = 0; i < boundaries.size(); ++i){
		boundaries.at(i)->setShader(_shader, _default);
	}
}

void Room::translatePhysical(glm::vec3 _v){
	btVector3 v(_v.x, _v.y, _v.z);

	for(unsigned int i = 0; i < boundaries.size(); ++i){
		boundaries.at(i)->body->translate(v);
	}
}

Room::~Room(void){
}
