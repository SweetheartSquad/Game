#pragma once
#include <string>
#include <vector>
#include <json/json.h>

class PD_FurnitureComponentDefinition {
public:
	std::string componentType;
	bool required;
	std::vector<PD_FurnitureComponentDefinition *> outComponents;
	int multiplier;	

	explicit PD_FurnitureComponentDefinition(Json::Value _jsonDef);
};
