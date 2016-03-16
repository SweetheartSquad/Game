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
PD_Scenario * PD_ResourceManager::itemTextures = nullptr;
PD_Scenario * PD_ResourceManager::componentTextures = nullptr;
std::vector<PD_FurnitureDefinition*> PD_ResourceManager::furnitureDefinitions;
PD_FurnitureComponentContainer * PD_ResourceManager::furnitureComponents = nullptr;
std::map<std::string, AnimationDef> PD_ResourceManager::characterAnimations;
ConditionImplementations * PD_ResourceManager::conditionImplementations = new ConditionImplementations();
std::map<std::string, std::vector<std::string>> PD_ResourceManager::characterDefinitions;
std::map<std::string, EmoteDef *> PD_ResourceManager::emotes;
std::vector<std::string> PD_ResourceManager::characterNames;
std::vector<PD_PropDefinition *> PD_ResourceManager::propDefinitions;
std::map<std::string, std::vector<PD_PropDefinition *>> PD_ResourceManager::furniturePropDefinitions;
std::vector<PD_PropDefinition *> PD_ResourceManager::independentPropDefinitions;
std::map<std::string, std::vector<std::string>> PD_ResourceManager::roomTypes;
std::vector<std::string> PD_ResourceManager::roomTypesKeys;
std::vector<OpenAL_Sound *> PD_ResourceManager::voices;

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

	{
		Json::Value root;
		Json::Reader reader;
		std::string jsonLoaded = sweet::FileUtils::readFile("assets/room-types.json");
		bool parsingSuccessful = reader.parse( jsonLoaded, root );
		if(!parsingSuccessful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
		}else{
			for(auto m : root.getMemberNames()) {
				for(auto t : root[m]) {
					roomTypes[m].push_back(t.asString());
					roomTypesKeys.push_back(m);
				}
			}
		}
	}

	// initialize assets
	scenario		  = new PD_Scenario("assets/scenario.json");
	itemTextures	  = new PD_Scenario("assets/item-textures.json");
	componentTextures = new PD_Scenario("assets/component-textures.json");

	resources.push_back(scenario);

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
			independentPropDefinitions.push_back(d);
		}
	}

	for(auto fDef : furnitureDefinitions){
		std::vector<PD_PropDefinition *> propDefs;

		for(auto pDef : PD_ResourceManager::propDefinitions){
			bool found = false;
			for(auto p :pDef->parents){
				if(p.parent == fDef->type){
					propDefs.push_back(pDef);
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
					AnimationDef def;
					def.steps = steps;
					def.canBeRandom = animation.get("random", true).asBool();
					characterAnimations[animation.get("name", "NO_NAME").asString()] = def;
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
					characterDefinitions[comp["type"].asString()].push_back(comp["src"].asString());
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
				characterNames.push_back(name.asString());
			}
		}
	}
	/*
	Uncomment if we want to actually use emotes
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
	*/
	for(unsigned long int i = 1; i < 37; ++i) {
		voices.push_back(PD_ResourceManager::scenario->getAudio("voice" + std::to_string(i))->sound);
	}

	load();
}

PD_ResourceManager::~PD_ResourceManager(){
	unload();

	for(auto e : emotes){
		delete e.second;
	}
	emotes.clear();

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

	delete itemTextures;
	delete componentTextures;

	destruct();
}