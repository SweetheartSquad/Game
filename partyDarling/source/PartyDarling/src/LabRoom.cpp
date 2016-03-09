#pragma once

#include <LabRoom.h>
#include <RoomBuilder.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <MeshFactory.h>

LabRoom::LabRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, Scenario * _labScenario) :
	Room(_world, _toonShader, dynamic_cast<AssetRoom *>(_labScenario->getAsset("room","1")))
{
	PD_Door * doorNorth = new PD_Door(world, _toonShader, PD_Door::kNORTH, 4);
	PD_Door * doorSouth = new PD_Door(world, _toonShader, PD_Door::kSOUTH, 4);
	PD_Door * doorEast = new PD_Door(world, _toonShader, PD_Door::kEAST, 4);
	PD_Door * doorWest = new PD_Door(world, _toonShader, PD_Door::kWEST, 4);

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

	doorNorth->translatePhysical(glm::vec3(0,0,-22));
	doorSouth->translatePhysical(glm::vec3(0,0,22));
	doorEast->translatePhysical(glm::vec3(22,0,0));
	doorWest->translatePhysical(glm::vec3(-22,0,0));
	
	
	TriMesh * mesh = PD_ResourceManager::scenario->getMesh("LAB-ROOM")->meshes.at(0);
	mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("LAB-ROOM")->texture);
	mesh->setScaleMode(GL_NEAREST);
	childTransform->addChild(new MeshEntity(mesh, _toonShader));
	setColliderAsMesh(PD_ResourceManager::scenario->getMesh("LAB-ROOM-COLLIDER")->meshes.at(0), false);
	createRigidBody(0);
	
	PD_Listing * listing = new PD_Listing(_labScenario);
	AssetCharacter * c = nullptr;
	auto it = _labScenario->assets["character"].begin();
	if(it != _labScenario->assets["character"].end()){
		c = dynamic_cast<AssetCharacter *>(it->second);
		if(c->id == "0"){
			++it;
			if(it != _labScenario->assets["character"].end()){
				c = dynamic_cast<AssetCharacter *>(it->second);
			}else{
				c = nullptr;
			}
		}
	}

	if(c != nullptr){
		PD_Character * p = new PD_Character(_world, c, MeshFactory::getPlaneMesh(3.f), _characterShader, _emoteShader);
		addComponent(p);
		characters.push_back(p);
		listing->addCharacter(p);
	}
	
	ceiling->translatePhysical(glm::vec3(0, ROOM_HEIGHT * ROOM_TILE, 0), false);
	
	ceiling->setVisible(false);
	floor->setVisible(false);

	removePhysics();
}