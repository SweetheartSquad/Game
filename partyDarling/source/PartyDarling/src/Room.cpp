#pragma once

#include <Room.h>
#include <math.h>

#include <MeshInterface.h>
#include <BulletMeshEntity.h>

#include <Sprite.h>
#include <Texture.h>
#include <RoomObject.h>
#include <RoomBuilder.h>
#include <PD_TilemapGenerator.h>

Room::Room(BulletWorld * _world, Shader * _shader, AssetRoom * const _definition) :
	BulletMeshEntity(_world, new QuadMesh(true), _shader),
	definition(_definition),
	tilemapSprite(new Sprite()),
	locked(definition->locked),
	visibility(kHIDDEN)
{
	mesh->setScaleMode(GL_NEAREST);
	doors.clear();
}

void Room::addComponent(RoomObject * _obj){
	components.push_back(_obj);
}

void Room::removeComponent(RoomObject * _obj){
	for(signed long int i = components.size()-1; i >= 0; --i){
		if(components.at(i) == _obj){
			components.erase(components.begin() + i);
			break;
		}
	}
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
	BulletMeshEntity::translatePhysical(_v, _relative);
	tilemapSprite->meshTransform->translate(_v, _relative);
	//TODO: move room to an absolute position while moving components relatively? (maintain component spacing)
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->translatePhysical(_v, _relative);
	}
}

Room::~Room(void){
}

glm::vec3 Room::getCenter() const{
	return glm::vec3((tilemap->width/2.f) * ROOM_TILE, 0.f, (tilemap->height/2.f) * ROOM_TILE);
}

void Room::removePhysics(){
	for(unsigned int i = 0; i < components.size(); ++i){
		world->world->removeRigidBody(components.at(i)->body);
	}

	world->world->removeRigidBody(body);
	world->world->removeRigidBody(floor->body);
	world->world->removeRigidBody(ceiling->body);
}

void Room::addPhysics(){
	for(unsigned int i = 0; i < components.size(); ++i){
		world->world->addRigidBody(components.at(i)->body);
	}

	world->world->addRigidBody(body);
	world->world->addRigidBody(floor->body);
	world->world->addRigidBody(ceiling->body);
}