#pragma once

#include <PD_ResourceManager.h>

#include <PD_Assets.h>

Scenario * PD_ResourceManager::scenario = nullptr;
Scenario * PD_ResourceManager::itemTextures = nullptr;
Scenario * PD_ResourceManager::componentTextures = nullptr;
DatabaseConnection * PD_ResourceManager::db = nullptr;
std::vector<PD_FurnitureDefinition*> PD_ResourceManager::furnitureDefinitions;
PD_FurnitureComponentContainer * PD_ResourceManager::furnitureComponents = nullptr;
std::map<std::string, std::vector<PD_CharacterAnimationStep>> PD_ResourceManager::characterAnimations;
ConditionImplementations * PD_ResourceManager::conditionImplementations = new ConditionImplementations();
std::map<std::string, std::vector<std::string>> PD_ResourceManager::characterDefinitions;

void PD_ResourceManager::init(){
	// register custom asset types
	Asset::registerType("item", &AssetItem::create);
	Asset::registerType("character", &AssetCharacter::create);
	Asset::registerType("room", &AssetRoom::create);

	// initialize assets
	scenario = new Scenario("assets/scenario.json");
	itemTextures = new Scenario("assets/item-textures.json");
	componentTextures = new Scenario("assets/component-textures.json");


	// add door asset manually
	for(unsigned long int i = 1; i <= 5; ++i){
		std::stringstream ss;
		ss << i;
		Json::Value root;
		Json::Reader reader;
		reader.parse("{\"id\": \"DOOR_" + ss.str() + "\",\"src\": \"items/door_" + ss.str() + ".png\",\"type\": \"texture\"}", root);
		itemTextures->assets["texture"]["DOOR_" + ss.str()] = Asset::getAsset(root, itemTextures);
		reader.parse("{\"name\":\"Door\",\"id\":\"DOOR_" + ss.str() + "\",\"collectable\":false,\"pixelPerfect\":true,\"description\":\"Who knows where it leads?\",\"texture\":\"DOOR_" + ss.str() + "\",\"effects\":[{\"type\":\"reset\",\"args\":{}}],\"type\":\"item\"}", root);
		scenario->assets["item"]["DOOR_" + ss.str()] = Asset::getAsset(root, scenario);
	}
	// parse furniture
	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/furniture.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
		}else{
			for(auto furnDef : root["furniture"]) {
				// parse the external json file
				Json::Value componentRoot;
				parsingSuccessful = reader.parse(sweet::FileUtils::readFile("assets/" + furnDef.get("src", "NO_SRC").asString()), componentRoot);
				if(!parsingSuccessful){
					Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
				}

				furnitureDefinitions.push_back(new PD_FurnitureDefinition(componentRoot));
			}
		}
	}
	furnitureComponents = new PD_FurnitureComponentContainer("assets/furniture.json");

	// Parse Animations
	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/animations.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
		}else{
			for(auto animation : root["animations"]) {
				Json::Value animStep;
				jsonLoaded = sweet::FileUtils::readFile("assets/animations/" + animation.get("src", "NO_SRC").asString());
				parsingSuccessful = reader.parse( jsonLoaded, animStep );
				if(!parsingSuccessful){
					Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
				}else{					
					std::vector<PD_CharacterAnimationStep> steps;
					for(auto step : animStep) {
						steps.push_back(PD_CharacterAnimationStep(step));	
					}
					characterAnimations[animation.get("name", "NO_NAME").asString()] = steps;
				}
			}
		}
	}
	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/component-definitions.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
		}else{
			for(auto comp : root["components"]) {
				characterDefinitions[comp["type"].asString()].push_back(comp["src"].asString());
			}
		}
	}
	

	db = new DatabaseConnection("data/test.db");

	resources.push_back(scenario);
}

int PD_ResourceManager::dbCallback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

void PD_ResourceManager::testSql(std::string _sql, bool _async){
	if(_async){
		db->queryDbAsync(_sql, dbCallback);
	}else{
		db->queryDb(_sql, dbCallback);
	}
}