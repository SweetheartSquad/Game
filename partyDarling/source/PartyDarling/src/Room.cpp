#pragma once

#include <Room.h>
#include <math.h>

#include <MeshInterface.h>
#include <BulletMeshEntity.h>
#include <Sprite.h>
#include <Texture.h>
#include <RoomObject.h>
#include <RoomBuilder.h>
#include <MeshFactory.h>

#include <PD_TilemapGenerator.h>
#include <PD_Masks.h>

Room::Room(BulletWorld * _world, Shader * _shader, AssetRoom * const _definition) :
	BulletMeshEntity(_world, new QuadMesh(true), _shader),
	definition(_definition),
	tilemap(nullptr),
	locked(definition->locked),
	visibility(kHIDDEN)
{
	mesh->setScaleMode(GL_NEAREST);
	doors.clear();

	// create floor/ceiling as static bullet planes
	floor = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), _shader);
	floor->setColliderAsStaticPlane(0, 1, 0, 0);
	floor->createRigidBody(0, kENVIRONMENT);
	floor->body->setFriction(1);
	childTransform->addChild(floor);
	floor->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	floor->body->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(180.f)));
	floor->mesh->setScaleMode(GL_NEAREST);

	ceiling = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), _shader);
	ceiling->setColliderAsStaticPlane(0, -1, 0, 0);
	ceiling->createRigidBody(0, kENVIRONMENT);
	ceiling->body->setFriction(1);
	childTransform->addChild(ceiling);
	ceiling->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	ceiling->body->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(180.f)));
	ceiling->mesh->setScaleMode(GL_NEAREST);
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
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->setShader(_shader, _default);
	}
}

void Room::translatePhysical(glm::vec3 _v, bool _relative){
	BulletMeshEntity::translatePhysical(_v, _relative);
	//TODO: move room to an absolute position while moving components relatively? (maintain component spacing)
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->translatePhysical(_v, _relative);
	}
}

Room::~Room(void){
	delete tilemap;
}

glm::vec3 Room::getCenter() const{
	sweet::Box boundingBox = mesh->calcBoundingBox();

	glm::vec3 center = glm::vec3(boundingBox.x + boundingBox.width * 0.5, 0, boundingBox.z + boundingBox.depth * 0.5);
	center += meshTransform->getTranslationVector();
	center *= meshTransform->getScaleVector();

	center += childTransform->getTranslationVector();
	center *= childTransform->getScaleVector();

	return center; //glm::vec3((tilemap->width/2.f) * ROOM_TILE, 0.f, (tilemap->height/2.f) * ROOM_TILE);
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

bool Room::removeItem(PD_Item * _item) {
	auto it = std::find(items.begin(), items.end(), _item);
	if(it != items.end()) {
		items.erase(it);
		return true;
	}
	return false;
}