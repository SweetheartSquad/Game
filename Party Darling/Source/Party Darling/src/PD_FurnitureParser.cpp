#pragma once

#include <PD_FurnitureParser.h>
#include <json/json.h>
#include <FileUtils.h>
#include <string>
#include <Log.h>
#include <PD_FurnitureDefinition.h>
#include <vector>
#include <PD_FurnitureComponent.h>

#define UNDEFINED "UNDEFINED"

std::vector<PD_FurnitureDefinition *> PD_FurnitureParser::parseFurnitureDefinitions() {
	
	std::vector<PD_FurnitureDefinition *> furnitureDefinitions;
	
	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = FileUtils::readFile("assets/furniture.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}else{
		for(auto furnDef : root["furniture"]) {
			furnitureDefinitions.push_back( new PD_FurnitureDefinition(furnDef));
		}
	}

	return furnitureDefinitions;
}


std::vector<PD_FurnitureComponent*> PD_FurnitureParser::parseFurnitureComponents() {

	std::vector<PD_FurnitureComponent *> componentDefinitions;
	
	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = FileUtils::readFile("assets/furniture.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}else{
		for(auto furnDef : root["components"]) {
			componentDefinitions.push_back( new PD_FurnitureComponent(furnDef));
		}
	}

	return componentDefinitions;
}
