#pragma once

#include <Room.h>
#include <math.h>

#include <MeshInterface.h>
#include <BulletMeshEntity.h>

#include <Sprite.h>
#include <Texture.h>
#include <RoomObject.h>
#include <RoomBuilder.h>

Room::Room(BulletWorld * _world, MeshInterface * _mesh):
	BulletMeshEntity(_world, _mesh),
	tilemapSprite(new Sprite())
{
}

void Room::addComponent(RoomObject * obj){
	components.push_back(obj);
}

void Room::setShader(Shader * _shader, bool _default){
	printHierarchy();
	BulletMeshEntity::setShader(_shader, _default);
	tilemapSprite->setShader(_shader, _default);
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->setShader(_shader, _default);
	}
}

void Room::translatePhysical(glm::vec3 _v, bool _relative){
	setTranslationPhysical(_v, true);
	tilemapSprite->meshTransform->translate(_v, true);
	//TODO: move room to an absolute position while moving components relatively? (maintain component spacing)
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->translatePhysical(_v, true);
	}
}

Room::~Room(void){
}
