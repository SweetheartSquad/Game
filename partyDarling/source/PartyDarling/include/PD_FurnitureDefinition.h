#pragma once

#include <string>
#include <json/json.h>

class PD_FurnitureComponentDefinition;

class PD_FurnitureDefinition {
public:

	std::string type;
	float mass;
	bool detailedCollider;
	std::vector<PD_FurnitureComponentDefinition *> components;

	explicit PD_FurnitureDefinition(Json::Value _jsonDef);
};
