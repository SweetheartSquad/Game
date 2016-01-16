#pragma once

#include <PD_FurnitureParser.h>
#include <json/json.h>
#include <FileUtils.h>
#include <string>
#include <Log.h>
#include <PD_FurnitureDefinition.h>
#include <vector>
#include <PD_FurnitureComponent.h>
#include <NumberUtils.h>

PD_FurnitureComponent * PD_FurnitureComponentContainer::getComponentForType(std::string _type) {
	if(componentsMap.find(_type) != componentsMap.end()) {
		int idx = sweet::NumberUtils::randomInt(0, componentsMap[_type].size() - 1);
		return componentsMap[_type].at(idx);
	}
	return nullptr;
}

std::vector<PD_FurnitureDefinition *> * PD_FurnitureParser::parseFurnitureDefinitions() {
	
	std::vector<PD_FurnitureDefinition *> * furnitureDefinitions = new std::vector<PD_FurnitureDefinition *>;
	
	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = FileUtils::readFile("assets/furniture.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}else{
		for(auto furnDef : root["furniture"]) {
			furnitureDefinitions->push_back( new PD_FurnitureDefinition(furnDef));
		}
	}
	return furnitureDefinitions;
}


PD_FurnitureComponentContainer * PD_FurnitureParser::parseFurnitureComponents() {

	PD_FurnitureComponentContainer * container = new PD_FurnitureComponentContainer();

	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = FileUtils::readFile("assets/furniture.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}else{
		for(auto furnDef : root["components"]) {
			auto comp = new PD_FurnitureComponent(furnDef);
			if(container->componentsMap.find(comp->type) == container->componentsMap.end()) {
				container->componentsMap[comp->type] = std::vector<PD_FurnitureComponent *>();
			}
			container->componentsMap[comp->type].push_back( new PD_FurnitureComponent(furnDef));
		}
	}
	return container;
}
