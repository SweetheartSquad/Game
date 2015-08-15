#pragma once

#include <Room.h>
#include <math.h>

#include <MeshInterface.h>
#include <BulletMeshEntity.h>

#include <Sprite.h>
#include <Texture.h>
#include <RoomObject.h>
#include <RoomBuilder.h>

Room::Room(BulletWorld * _world, RoomLayout_t _type, glm::vec2 _size, Texture * _wallTexture):
	tilemapSprite(new Sprite())
{

	// Create room boundaries (walls, floor, ceiling)
	boundaries = RoomBuilder::getBoundaries(_world, _type, _size);


	// TODO: make this a bullet body ground
	/*Texture * tex = new Texture("assets/textures/uvs_alt.png", true, true);
	tex->load();
	tilemapSprite->mesh->pushTexture2D(tex);
	tilemapSprite->childTransform->scale(1000, 1000, 1);
	tilemapSprite->meshTransform->translate(0.5, 0.5, 0);
	tilemapSprite->meshTransform->rotate(-45, 1, 0, 0, kOBJECT);
	tilemapSprite->mesh->scaleModeMag = GL_NEAREST;
	tilemapSprite->mesh->scaleModeMin = GL_NEAREST;
	//childTransform->addChild(tilemapSprite);
	*/
	for(unsigned int i = 0; i < boundaries.size(); ++i){
		addComponent(boundaries.at(i));
		if(_wallTexture != nullptr){
			boundaries.at(i)->mesh->pushTexture2D(_wallTexture);
		}
	}

	// Level with ground plane
	translatePhysical(glm::vec3(0, ROOM_HEIGHT * ROOM_TILE / 2.f - (1 - 0.05), 0));
}

void Room::addComponent(RoomObject * obj){
	childTransform->addChild(obj);
	components.push_back(obj);
}

void Room::setShader(Shader * _shader, bool _default){
	tilemapSprite->setShader(_shader, _default);
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->setShader(_shader, _default);
	}
}

void Room::translatePhysical(glm::vec3 _v, bool _relative){
	tilemapSprite->meshTransform->translate(_v, true);
	//TODO: move room to an absolute position while moving components relatively? (maintain component spacing)
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->translatePhysical(_v, true);
	}
}

Room::~Room(void){
}
