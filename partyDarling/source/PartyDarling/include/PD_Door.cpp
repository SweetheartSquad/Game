#pragma once

#include <PD_Door.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <Room.h>

#define DOOR_PADDING 0.5

PD_Door::PD_Door(BulletWorld * _world, Shader * _shader, Door_t _side, unsigned long int _doorIndex) :
	PD_Item(dynamic_cast<AssetItem *>(PD_ResourceManager::scenario->getAsset("item","DOOR_" + std::to_string(_doorIndex))), _world, _shader, Anchor_t::WALL),
	side(_side),
	room(nullptr)
{
	PD_ParentDef wallDef;
	wallDef.parent = "wall";
	wallDef.sides.push_back(PD_Side::kFRONT);
	parentTypes.push_back(wallDef);

	boundingBox.width *= 1.f + DOOR_PADDING * 2.f;
	boundingBox.x = -(boundingBox.width * 0.5f);

	boundingBox.depth = boundingBox.width * 0.1f;
	boundingBox.z = -boundingBox.depth * 0.5;

#ifdef _DEBUG
	boundingBoxMesh->meshTransform->scale(1.f + DOOR_PADDING * 2.f, 1, 1);
#endif
}

void PD_Door::triggerInteract(){
	PD_Item::triggerInteract();
	if(room->locked){
		sweet::Event * lockedEvent = new sweet::Event("locked");
		PD_ResourceManager::scenario->eventManager->triggerEvent(lockedEvent);
	}else{
		Log::info("Door: " + std::to_string(side));
		sweet::Event * navigationEvent = new sweet::Event("navigate");
		glm::ivec2 navigation;
		switch(side){
		case kNORTH:
			navigation.x = 0;
			navigation.y = 1;
			break;
		case kSOUTH:
			navigation.x = 0;
			navigation.y = -1;
			break;
		case kEAST:
			navigation.x = -1;
			navigation.y = 0;
			break;
		case kWEST:
			navigation.x = 1;
			navigation.y = 0;
			break;
		}
		navigationEvent->setIntData("x", navigation.x);
		navigationEvent->setIntData("y", navigation.y);
		PD_ResourceManager::scenario->eventManager->triggerEvent(navigationEvent);
	}
}