#pragma once

#include <IntroRoom.h>
#include <RoomBuilder.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <MeshFactory.h>

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

	doorNorth->translatePhysical(glm::vec3(0,0,-11.8));
	doorSouth->translatePhysical(glm::vec3(0,0,11.8));
	doorEast->translatePhysical(glm::vec3(11.8,0,0));
	doorWest->translatePhysical(glm::vec3(-11.8,0,0));
	
	doorEast->rotatePhysical(-90, 0, 1, 0);
	doorWest->rotatePhysical(90, 0, 1, 0);
	doorSouth->rotatePhysical(180, 0, 1, 0);

	TriMesh * mesh = PD_ResourceManager::introScenario->getMesh("INTRO-ROOM")->meshes.at(0);
	childTransform->addChild(new MeshEntity(mesh, _toonShader));
	setColliderAsMesh(mesh, false);
	createRigidBody(0);

	AssetCharacter * c = dynamic_cast<AssetCharacter *>(PD_ResourceManager::introScenario->getAsset("character", "Butler"));
	Person * p = new Person(_world, c, MeshFactory::getPlaneMesh(3.f), _characterShader, _emoteShader);
	addComponent(p);
	characters.push_back(p);
	PD_Listing * listing = new PD_Listing(PD_ResourceManager::introScenario);
	listing->addCharacter(p);
	
	ceiling->translatePhysical(glm::vec3(0, ROOM_HEIGHT * ROOM_TILE, 0), false);
	
	ceiling->meshTransform->scale(glm::vec3(24, 24, 1), false);
	floor->meshTransform->scale(glm::vec3(24, 24, 1), false);

	removePhysics();
}