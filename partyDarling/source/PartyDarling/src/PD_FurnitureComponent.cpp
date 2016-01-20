#pragma once

#include <PD_FurnitureComponent.h>
#include <Resource.h>

PD_FurnitureComponent::PD_FurnitureComponent(Json::Value _jsonDef) :
	id(_jsonDef.get("id", -1).asInt()),
	type(_jsonDef.get("type", -1).asString()),
	src(_jsonDef.get("src", "NO_SRC").asString()),
	mesh(Resource::loadMeshFromObj("assets/meshes/furniture/" + src).at(0)) // TODO: maybe don't do this until the first time it's loaded?
{	
	for(auto jsonObj : _jsonDef["connectors"]) {
		// create the component types key
		std::vector<std::string> compTypes;
		for(auto compType : jsonObj["componentTypes"]){
			compTypes.push_back(compType.asString());
		}
		

		// store the position, rotation, and scale
		for(auto jnt : jsonObj["out"]){
			connectors[compTypes].push_back(PD_FurnitureConnector());
			connectors[compTypes].back().position = glm::vec3(
				jnt["position"].get(Json::Value::ArrayIndex(0), 0.5f).asFloat(),
				jnt["position"].get(Json::Value::ArrayIndex(1), 0.5f).asFloat(),
				jnt["position"].get(Json::Value::ArrayIndex(2), 0.5f).asFloat());
			connectors[compTypes].back().scale = glm::vec3(
				jnt["scale"].get(Json::Value::ArrayIndex(0), 0.5f).asFloat(),
				jnt["scale"].get(Json::Value::ArrayIndex(1), 0.5f).asFloat(),
				jnt["scale"].get(Json::Value::ArrayIndex(2), 0.5f).asFloat());
			connectors[compTypes].back().rotation = glm::vec3(
				jnt["rotation"].get(Json::Value::ArrayIndex(0), 0.5f).asFloat(),
				jnt["rotation"].get(Json::Value::ArrayIndex(1), 0.5f).asFloat(),
				jnt["rotation"].get(Json::Value::ArrayIndex(2), 0.5f).asFloat());
		}
	}
}
