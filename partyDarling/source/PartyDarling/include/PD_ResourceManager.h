#pragma once

#include <ResourceManager.h>

#include <scenario\Scenario.h>
#include <PD_PropDefinition.h>
#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureParser.h>
#include <map>
#include <PD_CharacterAnimationStep.h>
#include "PD_Assets.h"
#include <NumberUtils.h>
#include <string>
#include "PD_Listing.h"
#include "PD_Scenario.h"

#define PLAYER_ID "0"

struct EmoteDef {
	SpriteSheet * spriteSheet;
	glm::vec2 offset;

	~EmoteDef();
};

struct AnimationDef {
	std::vector<PD_CharacterAnimationStep> steps;
	bool canBeRandom;
};

class PD_ResourceManager : public ResourceManager{
public:
	static PD_Scenario * scenario;
	static PD_Scenario * itemTextures;
	static PD_Scenario * componentTextures;
	static std::vector<PD_FurnitureDefinition *> furnitureDefinitions;
	static PD_FurnitureComponentContainer * furnitureComponents;
	static std::map<std::string, AnimationDef> characterAnimations;
	static ConditionImplementations * conditionImplementations;
	static std::map<std::string, std::vector<std::string>> characterDefinitions;
	static std::map<std::string, EmoteDef *> emotes;
	static std::vector<std::string> characterNames;

	static std::vector<PD_PropDefinition *> propDefinitions;
	static std::map<std::string, std::vector<PD_PropDefinition *>> furniturePropDefinitions;
	static std::vector<PD_PropDefinition *> independentPropDefinitions;
	static std::map<std::string, std::vector<std::string>> roomTypes;
	static std::vector<std::string> roomTypesKeys;
	static std::vector<OpenAL_Sound *> voices;

	PD_ResourceManager();
	~PD_ResourceManager();
};
