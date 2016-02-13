#pragma once

#include <ResourceManager.h>

#include <scenario\Scenario.h>
#include <DatabaseConnection.h>
#include <PD_PropDefinition.h>
#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureParser.h>
#include <map>
#include <PD_CharacterAnimationStep.h>
#include "PD_Assets.h"
#include <NumberUtils.h>
#include <string>
#include "PD_Listing.h"

#define PLAYER_ID "0"

struct EmoteDef {
	SpriteSheet * spriteSheet;
	glm::vec2 offset;
};

class PD_ResourceManager : public ResourceManager{
public:
	static Scenario * scenario;
	static Scenario * itemTextures;
	static Scenario * componentTextures;
	static DatabaseConnection * db;
	static std::vector<PD_FurnitureDefinition *> furnitureDefinitions;
	static PD_FurnitureComponentContainer * furnitureComponents;
	static std::map<std::string, std::vector<PD_CharacterAnimationStep>> characterAnimations;
	static ConditionImplementations * conditionImplementations;
	static std::map<std::string, sweet::ShuffleVector<std::string>> characterDefinitions;
	static std::map<std::string, EmoteDef> emotes;
	static sweet::ShuffleVector<std::string> characterNames;
	static PD_Listing * globalScenarioListing;

	static std::vector<PD_PropDefinition *> propDefinitions;

	static void init();

	static int dbCallback(void *NotUsed, int argc, char **argv, char **azColName);
	static void testSql(std::string _sql, bool _async);
};
