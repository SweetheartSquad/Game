#pragma once

#include <PD_FurnitureComponentDefinition.h>
#include <string>
#include <json/json.h>

class PD_FurnitureDefinition {
public:

	std::string type;
	std::vector<PD_FurnitureComponentDefinition *> components;

	explicit PD_FurnitureDefinition(Json::Value _jsonDef);
};
