#pragma once

#include <PD_FurnitureComponent.h>
#include <Resource.h>

PD_FurnitureConnector::PD_FurnitureConnector(Json::Value _jsonDef) {
	description =_jsonDef.get("description", "UNDEFINED").asString();
	positionX = _jsonDef.get("positionX", 0.0f).asFloat();
	positionY = _jsonDef.get("positionY", 0.0f).asFloat();
	positionZ = _jsonDef.get("positionZ", 0.0f).asFloat();
}

PD_FurnitureComponent::PD_FurnitureComponent(Json::Value _jsonDef) {
	id = _jsonDef.get("id", -1).asInt();
	type = _jsonDef.get("type", -1).asString();
	src = _jsonDef.get("src", "UNDEFINED").asString();

	if(src != "UNDEFINED") {
		mesh = Resource::loadMeshFromObj("assets/meshes/furniture/" + src).at(0);
	}

	for(auto jsonObj : _jsonDef["connectors"]) {
		connectors.push_back(new PD_FurnitureConnector(jsonObj));
	}
}
