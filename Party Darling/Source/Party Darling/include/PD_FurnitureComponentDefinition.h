#pragma once
#include <string>
#include <vector>
#include <json/json.h>
#include "PD_FurnitureParser.h"

class MeshEntity;

class PD_FurnitureComponentDefinition {
public:
	std::string componentType;
	bool required;
	std::vector<PD_FurnitureComponentDefinition *> outComponents;
	std::vector<int>multipliers;
	int multiplier;	

	explicit PD_FurnitureComponentDefinition(Json::Value _jsonDef);
	
	MeshEntity * buildChildren(PD_FurnitureComponentContainer * _componentContainer, int _multiplier, std::vector<glm::vec3> _positions) const;
};
