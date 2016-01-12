#pragma once

#include <PD_FurnitureComponent.h>
#include <Resource.h>

PD_FurnitureComponent::PD_FurnitureComponent(Json::Value _jsonDef) {
	id = _jsonDef.get("id", -1).asInt();
	type = _jsonDef.get("type", -1).asString();
	src = _jsonDef.get("src", "UNDEFINED").asString();

	if(src != "UNDEFINED") {
		mesh = Resource::loadMeshFromObj("assets/meshes/furniture/" + src).at(0);
	}

	for(auto jsonObj : _jsonDef["connectors"]) {
		connectors[jsonObj.get("description", "UNDEFINED").asString()] = glm::vec3(
			jsonObj.get("positionX", 0.0f).asFloat(),
			jsonObj.get("positionY", 0.0f).asFloat(),
			jsonObj.get("positionZ", 0.0f).asFloat());
	}
}
