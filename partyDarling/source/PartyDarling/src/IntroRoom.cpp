#pragma once

#include <IntroRoom.h>
#include <RoomBuilder.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <MeshFactory.h>

#include <PointLight.h>

IntroRoom::IntroRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, AssetRoom * const _definition) :
	Room(_world, _toonShader, _definition)
{
	PD_Door * doorNorth = new PD_Door(world, _toonShader, PD_Door::kNORTH, 1);
	PD_Door * doorSouth = new PD_Door(world, _toonShader, PD_Door::kSOUTH, 1);
	PD_Door * doorEast = new PD_Door(world, _toonShader, PD_Door::kEAST, 1);
	PD_Door * doorWest = new PD_Door(world, _toonShader, PD_Door::kWEST, 1);

	doors.insert(std::make_pair(PD_Door::kNORTH, doorNorth));
	doors.insert(std::make_pair(PD_Door::kSOUTH, doorSouth));
	doors.insert(std::make_pair(PD_Door::kEAST, doorEast));
	doors.insert(std::make_pair(PD_Door::kWEST, doorWest));
	
	for(auto d : doors){
		//childTransform->addChild(d.second);
		addComponent(d.second);
	}
	
	doorEast->rotatePhysical(-90, 0, 1, 0);
	doorWest->rotatePhysical(90, 0, 1, 0);
	doorSouth->rotatePhysical(180, 0, 1, 0);

	TriMesh * mesh = PD_ResourceManager::introScenario->getMesh("INTRO-ROOM")->meshes.at(0);
	mesh->pushTexture2D(PD_ResourceManager::introScenario->getTexture("INTRO-ROOM")->texture);
	mesh->setScaleMode(GL_NEAREST);
	visibleMesh = new MeshEntity(mesh, _toonShader);
	childTransform->addChild(visibleMesh);

	AssetCharacter * c = dynamic_cast<AssetCharacter *>(PD_ResourceManager::introScenario->getAsset("character", "Butler"));
	PD_Character * p = new PD_Character(_world, c, MeshFactory::getPlaneMesh(3.f), _characterShader, _emoteShader);
	addComponent(p);
	characters.push_back(p);
	PD_Listing * listing = new PD_Listing(PD_ResourceManager::introScenario);
	listing->addCharacter(p);
	
	ceiling->translatePhysical(glm::vec3(0, ROOM_HEIGHT * ROOM_TILE, 0), false);
	
	ceiling->setVisible(false);
	floor->setVisible(false);

	// lights
	{
		PointLight * light = new PointLight(glm::vec3(4.0f), 0.0f, 0.099f, -1);
		visibleMesh->childTransform->addChild(light)->translate(glm::vec3(-5.19, 1.508, 7.874));
		lights.push_back(light);
	}{
		PointLight * light = new PointLight(glm::vec3(4.0f), 0.0f, 0.099f, -1);
		visibleMesh->childTransform->addChild(light)->translate(glm::vec3(4.703, 1.508, 5.587));
		lights.push_back(light);
	}
}

void IntroRoom::setEdge(PD_Door::Door_t _edge){
	float forward = 2.5f;
	float backward = -30.f;
	float left = -5.8f;
	float right = 5.8f;
	TriMesh * colliderMesh = PD_ResourceManager::introScenario->getMesh("INTRO-ROOM-COLLIDER")->meshes.at(0);
	Transform t;

	switch(_edge){
	case PD_Door::kNORTH:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,-forward));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,-backward));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(right,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(left,0,0));
		//t.rotate(180, 0, 1, 0, kOBJECT);
		//visibleMesh->firstParent()->rotate(180, 0, 1, 0, kOBJECT);
		break;
	case PD_Door::kSOUTH:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,backward));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,forward));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(right,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(left,0,0));
		t.rotate(180, 0, 1, 0, kOBJECT);
		visibleMesh->firstParent()->rotate(180, 0, 1, 0, kOBJECT);
		break;
	case PD_Door::kWEST:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,left));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,right));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(-backward,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(-forward,0,0));
		t.rotate(90, 0, 1, 0, kOBJECT);
		visibleMesh->firstParent()->rotate(90, 0, 1, 0, kOBJECT);
		break;
	case PD_Door::kEAST:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,left));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,right));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(forward,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(backward,0,0));
		t.rotate(-90, 0, 1, 0, kOBJECT);
		visibleMesh->firstParent()->rotate(-90, 0, 1, 0, kOBJECT);
		break;

	}
	
	colliderMesh->applyTransformation(&t);
	setColliderAsMesh(colliderMesh, false);
	createRigidBody(0);

	removePhysics();
}