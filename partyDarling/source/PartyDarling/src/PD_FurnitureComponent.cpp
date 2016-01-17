#pragma once

#include <PD_FurnitureComponent.h>
#include <Resource.h>

PD_FurnitureComponent::PD_FurnitureComponent(Json::Value _jsonDef) :
	id(_jsonDef.get("id", -1).asInt()),
	type(_jsonDef.get("type", -1).asString()),
	src(_jsonDef.get("src", "UNDEFINED").asString()),
	mesh(Resource::loadMeshFromObj("assets/meshes/furniture/" + src).at(0))
{
	
	for(auto jsonObj : _jsonDef["connectors"]) {
		std::string compType = jsonObj.get("componentType", "UNDEFINED").asString();
		connectors[compType] = std::vector<glm::vec3>();
		

		for(auto jnt : jsonObj["positions"]){
			connectors[compType].push_back(glm::vec3(
				jnt.get(Json::Value::ArrayIndex(0), 0.5f).asFloat(),
				jnt.get(Json::Value::ArrayIndex(1), 0.5f).asFloat(),
				jnt.get(Json::Value::ArrayIndex(2), 0.5f).asFloat()));
		}
	}
}
