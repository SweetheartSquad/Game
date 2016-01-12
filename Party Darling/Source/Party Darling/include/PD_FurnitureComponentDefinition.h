#pragma once
#include <string>
#include <vector>
#include <json/json.h>
#include "PD_FurnitureParser.h"

class PD_FurnitureComponentDefinition {
public:
	std::string componentType;
	bool required;
	std::vector<PD_FurnitureComponentDefinition *> outComponents;
	int multiplier;	

	explicit PD_FurnitureComponentDefinition(Json::Value _jsonDef);
	
	PD_FurnitureComponent * build(PD_FurnitureComponentContainer * _componentContainer);
};
