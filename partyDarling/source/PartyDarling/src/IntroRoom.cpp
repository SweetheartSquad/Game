#pragma once

#include <IntroRoom.h>
#include <RoomBuilder.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <PD_Masks.h>
#include <MeshFactory.h>

#include <PointLight.h>

IntroRoom::IntroRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, Scenario * _introScenario) :
	Room(_world, _toonShader, dynamic_cast<AssetRoom *>(_introScenario->getAsset("room","1")))
{
	PD_Door * doorNorth = new PD_Door(world, _toonShader, PD_Door::kNORTH, 5);
	PD_Door * doorSouth = new PD_Door(world, _toonShader, PD_Door::kSOUTH, 5);
	PD_Door * doorEast = new PD_Door(world, _toonShader, PD_Door::kEAST, 5);
	PD_Door * doorWest = new PD_Door(world, _toonShader, PD_Door::kWEST, 5);

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

	TriMesh * meshFlats = new TriMesh(true);
	meshFlats->insertVertices(*PD_ResourceManager::scenario->getMesh("INTRO-ROOM")->meshes.at(0));
	TriMesh * meshDetail = new TriMesh(true);
	meshDetail->insertVertices(*PD_ResourceManager::scenario->getMesh("INTRO-ROOM")->meshes.at(1));
	meshFlats->pushTexture2D(PD_ResourceManager::scenario->getTexture("INTRO-ROOM-FLATS")->texture);
	meshFlats->setScaleMode(GL_NEAREST);
	meshDetail->pushTexture2D(PD_ResourceManager::scenario->getTexture("INTRO-ROOM-DETAIL")->texture);
	meshDetail->setScaleMode(GL_NEAREST);
	
	visibleMeshFlats = new MeshEntity(meshFlats, _toonShader);
	visibleMeshDetail = new MeshEntity(meshDetail, _toonShader);
	childTransform->addChild(visibleMeshFlats, false);
	childTransform->addChild(visibleMeshDetail, false);

	lightsContainer = new Transform();
	childTransform->addChild(lightsContainer, true);

	colliderMesh = new TriMesh(false);
	colliderMesh->insertVertices(*PD_ResourceManager::scenario->getMesh("INTRO-ROOM-COLLIDER")->meshes.at(0));

	PD_Listing * listing = PD_Listing::listings[_introScenario];
	AssetCharacter * c = nullptr;
	auto it = _introScenario->assets["character"].begin();
	if(it != _introScenario->assets["character"].end()){
		c = dynamic_cast<AssetCharacter *>(it->second);
		if(c->id == "0"){
			++it;
			if(it != _introScenario->assets["character"].end()){
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

	// lights
	{
		PointLight * light = new PointLight(glm::vec3(4.0f), 0.0f, 0.099f, -1);
		lightsContainer->addChild(light)->translate(glm::vec3(-5.19, 1.508, 7.874));
		lights.push_back(light);
	}{
		PointLight * light = new PointLight(glm::vec3(4.0f), 0.0f, 0.099f, -1);
		lightsContainer->addChild(light)->translate(glm::vec3(4.703, 1.508, 5.587));
		lights.push_back(light);
	}
}

IntroRoom::~IntroRoom(){
	delete colliderMesh;
}

void IntroRoom::setEdge(PD_Door::Door_t _edge){
	float forward = 2.5f;
	float backward = -40.f;
	float left = -5.8f;
	float right = 5.8f;
	Transform t;
	float angle = 0;
	switch(_edge){
	case PD_Door::kNORTH:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,-forward));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,-backward));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(right,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(left,0,0));
		angle = 0;
		break;
	case PD_Door::kSOUTH:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,backward));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,forward));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(right,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(left,0,0));
		angle = 180;
		break;
	case PD_Door::kWEST:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,left));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,right));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(-backward,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(-forward,0,0));
		angle = 90;
		break;
	case PD_Door::kEAST:
		doors[PD_Door::kNORTH]->translatePhysical(glm::vec3(0,0,left));
		doors[PD_Door::kSOUTH]->translatePhysical(glm::vec3(0,0,right));
		doors[PD_Door::kEAST]->translatePhysical(glm::vec3(forward,0,0));
		doors[PD_Door::kWEST]->translatePhysical(glm::vec3(backward,0,0));
		angle = -90;
		break;
	}

	if(angle != 0){
		t.rotate(angle, 0, 1, 0, kOBJECT);
		visibleMeshFlats->childTransform->rotate(angle, 0, 1, 0, kOBJECT);
		visibleMeshDetail->childTransform->rotate(angle, 0, 1, 0, kOBJECT);
		lightsContainer->firstParent()->rotate(angle, 0, 1, 0, kOBJECT);
		visibleMeshFlats->freezeTransformation();
		visibleMeshDetail->freezeTransformation();
	}

	colliderMesh->applyTransformation(&t);
	setColliderAsMesh(colliderMesh, false);
	createRigidBody(0, kENVIRONMENT);

	removePhysics();
}