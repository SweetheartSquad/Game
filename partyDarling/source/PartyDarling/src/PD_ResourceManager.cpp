#pragma once

#include <PD_ResourceManager.h>

#include <PD_Assets.h>
#include <PD_Listing.h>
#include <SpriteSheet.h>
#include <PD_Scenario.h>

EmoteDef::~EmoteDef(){
	delete spriteSheet;
}

PD_Scenario * PD_ResourceManager::scenario = nullptr;
PD_Scenario * PD_ResourceManager::introScenario = nullptr;
PD_Scenario * PD_ResourceManager::labScenario = nullptr;
PD_Scenario * PD_ResourceManager::itemTextures = nullptr;
PD_Scenario * PD_ResourceManager::componentTextures = nullptr;
DatabaseConnection * PD_ResourceManager::db = nullptr;
std::vector<PD_FurnitureDefinition*> PD_ResourceManager::furnitureDefinitions;
PD_FurnitureComponentContainer * PD_ResourceManager::furnitureComponents = nullptr;
std::map<std::string, std::vector<PD_CharacterAnimationStep>> PD_ResourceManager::characterAnimations;
ConditionImplementations * PD_ResourceManager::conditionImplementations = new ConditionImplementations();
std::map<std::string, sweet::ShuffleVector<std::string>> PD_ResourceManager::characterDefinitions;
std::map<std::string, EmoteDef *> PD_ResourceManager::emotes;
sweet::ShuffleVector<std::string> PD_ResourceManager::characterNames;
PD_Listing * PD_ResourceManager::globalScenarioListing;
std::vector<PD_PropDefinition *> PD_ResourceManager::propDefinitions;
std::map<std::string, sweet::ShuffleVector<PD_PropDefinition *>> PD_ResourceManager::furniturePropDefinitions;
sweet::ShuffleVector<PD_PropDefinition *> PD_ResourceManager::independentPropDefinitions;

PD_ResourceManager::PD_ResourceManager(){
	// register custom asset types
	Asset::registerType("item", &AssetItem::create);
	Asset::registerType("character", &AssetCharacter::create);
	Asset::registerType("room", &AssetRoom::create);

	// Register custom argument tyes
	sweet::Event::registerArgumentType("ITEM",			  sweet::STRING);
	sweet::Event::registerArgumentType("ROOM",			  sweet::STRING);
	sweet::Event::registerArgumentType("CONVERSATION",	  sweet::STRING);
	sweet::Event::registerArgumentType("CHARACTER_STATE", sweet::STRING);
	sweet::Event::registerArgumentType("CHARACTER",		  sweet::STRING);

	// initialize assets
	scenario		  = new PD_Scenario("assets/scenario.json");
	introScenario     = new PD_Scenario("assets/intro-scenario.json");
	labScenario		  = new PD_Scenario("assets/lab-scenario.json");
	itemTextures	  = new PD_Scenario("assets/item-textures.json");
	componentTextures = new PD_Scenario("assets/component-textures.json");
	
	resources.push_back(scenario);
	resources.push_back(introScenario);
	resources.push_back(labScenario);
	resources.push_back(itemTextures);
	resources.push_back(componentTextures);

	// add door asset manually
	for(unsigned long int i = 1; i <= 5; ++i){
		std::stringstream ss;
		ss << i;
		Json::Value root;
		Json::Reader reader;
		reader.parse("{\"id\": \"DOOR_" + ss.str() + "\",\"src\": \"items/door_" + ss.str() + ".png\",\"type\": \"texture\"}", root);
		itemTextures->assets["texture"]["DOOR_" + ss.str()] = Asset::getAsset(root, itemTextures);
		reader.parse("{\"name\":\"Door\",\"id\":\"DOOR_" + ss.str() + "\",\"collectable\":false,\"pixelPerfect\":true,\"description\":\"Who knows where it leads?\",\"texture\":\"DOOR_" + ss.str() + "\",\"effects\":[],\"type\":\"item\"}", root);
		scenario->assets["item"]["DOOR_" + ss.str()] = Asset::getAsset(root, scenario);
	}
	// parse furniture
	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/furniture.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
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

	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/props.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
		}else{
			for(auto propDef : root["props"]) {
				propDefinitions.push_back(new PD_PropDefinition(propDef));
			}
		}
	}

	for(auto d : propDefinitions){
		if(!d->parentDependent){
			independentPropDefinitions.push(d);
		}
	}

	for(auto fDef : furnitureDefinitions){
		sweet::ShuffleVector<PD_PropDefinition *> propDefs;

		for(auto pDef : PD_ResourceManager::propDefinitions){
			bool found = false;
			for(auto p :pDef->parents){
				if(p.parent == fDef->type){
					propDefs.push(pDef);
					break;
				}
			}
		}
		furniturePropDefinitions.insert(std::make_pair(fDef->type, propDefs));
	}

	// Parse Animations
	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/animations.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
		}else{
			for(auto animation : root["animations"]) {
				Json::Value animStep;
				jsonLoaded = sweet::FileUtils::readFile("assets/animations/" + animation.get("src", "NO_SRC").asString());
				parsingSuccessful = reader.parse( jsonLoaded, animStep );
				if(!parsingSuccessful){
					Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
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
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
		}else{
			for(auto comp : root["components"]) {
				if(comp.get("random", true).asBool()){
					characterDefinitions[comp["type"].asString()].push(comp["src"].asString());
				}
			}
		}
	}
	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/characterNames.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
		}else{
			for(auto name : root["names"]) {
				characterNames.push(name.asString());
			}
		}
	}
	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/emotes.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
		}else{
			for(auto emote : root) {
				auto tex = new Texture("assets/textures/emotes/" + emote["src"].asString(), true, true);
				tex->load();
				SpriteSheet * spriteSheet = new SpriteSheet(tex);
				SpriteSheetAnimation * mainAnim = new SpriteSheetAnimation(emote["secondsPerFrame"].asFloat());
				mainAnim->pushFramesInRange(
					0, emote["frames"].asInt() - 1, 
					emote["frameWidth"].asInt(), 
					emote["frameHeight"].asInt(),
					tex->width, tex->height);
				spriteSheet->addAnimation("main", mainAnim);

				EmoteDef * def = emotes[emote["name"].asString()] = new EmoteDef();
				def->spriteSheet = spriteSheet;
				def->offset = glm::vec2(emote["offset"][0].asFloat(), emote["offset"][1].asFloat());
			}
		}
	}

	db = new DatabaseConnection("data/test.db");
	
	globalScenarioListing = new PD_Listing(scenario);
	

	load();
}

PD_ResourceManager::~PD_ResourceManager(){
	unload();
	
	delete globalScenarioListing;
	for(auto e : emotes){
		delete e.second;
	}
	emotes.clear();

	delete db;

	while(furnitureDefinitions.size() > 0){
		delete furnitureDefinitions.back();
		furnitureDefinitions.pop_back();
	}
	delete furnitureComponents;
	delete conditionImplementations;

	while(propDefinitions.size() > 0){
		delete propDefinitions.back();
		propDefinitions.pop_back();
	}

	destruct();
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