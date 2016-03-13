#pragma once

#include <LabRoom.h>
#include <RoomBuilder.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <MeshFactory.h>

#include <PointLight.h>

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
	
	
	std::vector<TriMesh *> meshes = PD_ResourceManager::scenario->getMesh("LAB-ROOM")->meshes;

	for(auto m : meshes){
		m->pushTexture2D(PD_ResourceManager::scenario->getTexture("LAB-ROOM-FLATS")->texture);
		m->setScaleMode(GL_NEAREST);
		toRotate.push_back(childTransform->addChild(new MeshEntity(m, _toonShader)));
	}
	meshes.at(meshes.size()-1)->replaceTextures(PD_ResourceManager::scenario->getTexture("LAB-ROOM-DETAIL")->texture);
	meshes.at(meshes.size()-2)->replaceTextures(PD_ResourceManager::scenario->getTexture("LAB-ROOM-DETAIL")->texture);
	meshes.at(meshes.size()-3)->replaceTextures(PD_ResourceManager::scenario->getTexture("LAB-ROOM-FLOATERS")->texture);

	setColliderAsMesh(PD_ResourceManager::scenario->getMesh("LAB-ROOM-COLLIDER")->meshes.at(0), false);
	createRigidBody(0);
	
	PD_Listing * listing = PD_Listing::listings[_labScenario];
	AssetCharacter * c1, * c2 = nullptr;
	auto it = _labScenario->assets["character"].begin();
	if(it != _labScenario->assets["character"].end()){
		c1 = dynamic_cast<AssetCharacter *>(it->second);
		if(c1->id == "0"){
			++it;
			if(it != _labScenario->assets["character"].end()){
				c1 = dynamic_cast<AssetCharacter *>(it->second);
			}else{
				c1 = nullptr;
			}
		}
	}

	if(c1 != nullptr){
		PD_Character * p = new PD_Character(_world, c1, MeshFactory::getPlaneMesh(3.f), _characterShader, _emoteShader);
		addComponent(p);
		characters.push_back(p);
		listing->addCharacter(p);

		++it;
		if(it != _labScenario->assets["character"].end()){
			c2 = dynamic_cast<AssetCharacter *>(it->second);
		}else{
			c2 = nullptr;
		}

		if(c2 != nullptr){
			PD_Character * p = new PD_Character(_world, c2, MeshFactory::getPlaneMesh(3.f), _characterShader, _emoteShader);
			addComponent(p);
			characters.push_back(p);
			listing->addCharacter(p);
		}
	}
	
	ceiling->translatePhysical(glm::vec3(0, ROOM_HEIGHT * ROOM_TILE, 0), false);
	
	ceiling->setVisible(false);
	floor->setVisible(false);
	
	topLight = new PointLight(glm::vec3(4.0f), 0.0f, 0.099f, -1);
	lights.push_back(topLight);
	childTransform->addChild(topLight);
	bottomLight = new PointLight(glm::vec3(4.0f), 0.0f, 0.099f, -1);
	lights.push_back(bottomLight);
	childTransform->addChild(bottomLight);

	removePhysics();
}

void LabRoom::update(Step * _step){
	for(unsigned long int i = 0; i < toRotate.size(); ++i){
		toRotate.at(i)->rotate(_step->deltaTime * i * (i % 2 == 0 ? -5.f : 5.f), 0,1,0, kOBJECT);
	}

	float lRadius = 4;
	float lOffset = 8;
	
	topLight->firstParent()->translate(glm::vec3(sin(_step->time)* lRadius, lOffset, cos(_step->time) * lRadius), false);
	bottomLight->firstParent()->translate(glm::vec3(sin(_step->time+glm::pi<float>())* lRadius, -lOffset, cos(_step->time+glm::pi<float>()) * lRadius), false);

	Room::update(_step);
};