#pragma once

#include <Room.h>
#include <math.h>

#include <MeshInterface.h>
#include <BulletMeshEntity.h>

#include <RoomObject.h>
#include <RoomBuilder.h>

Room::Room(BulletWorld * _world, RoomLayout_t _type, glm::vec2 _size, Texture * _wallTexture){

	// Create room boundaries (walls, floor, ceiling)
	boundaries = RoomBuilder::getBoundaries(_world, _type, _size);

	for(unsigned int i = 0; i < boundaries.size(); ++i){
		addComponent(boundaries.at(i));
		if(_wallTexture != nullptr){
			boundaries.at(i)->mesh->pushTexture2D(_wallTexture);
		}
	}
}

void Room::addComponent(RoomObject * obj){
	childTransform->addChild(obj);
	components.push_back(obj);
}

void Room::setShader(Shader * _shader, bool _default){
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->setShader(_shader, _default);
	}
		/*
	for(unsigned int i = 0; i < boundaries.size(); ++i){
		boundaries.at(i)->setShader(_shader, _default);
	}

	for(unsigned int i = 0; i < objects.size(); ++i){
		objects.at(i)->setShader(_shader, _default);
	}*/
}

void Room::translatePhysical(glm::vec3 _v){
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->translatePhysical(_v);
	}
	
	/*
	btVector3 v(_v.x, _v.y, _v.z);

	for(unsigned int i = 0; i < boundaries.size(); ++i){
		boundaries.at(i)->body->translate(v);
	}

	for(unsigned int i = 0; i < objects.size(); ++i){
		objects.at(i)->body->translate(v);
	}*/
}

Room::~Room(void){
}
