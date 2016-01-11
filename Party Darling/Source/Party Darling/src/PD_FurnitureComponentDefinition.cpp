#pragma once

#include <PD_FurnitureComponentDefinition.h>

PD_FurnitureComponentDefinition::PD_FurnitureComponentDefinition(Json::Value _jsonDef) {
	componentType = _jsonDef.get("componentType", "UNDEFINED").asString();
	bool required = _jsonDef.get("required", true).asBool();
	int multiplier = _jsonDef.get("multiplier", 1).asInt();

	for(auto jsonComp : _jsonDef["outComponents"]) {
		outComponents.push_back(new PD_FurnitureComponentDefinition(jsonComp));
	}
}

PD_FurnitureComponent * PD_FurnitureComponentDefinition::build(PD_FurnitureComponentContainer * _componentContainer) {
	PD_FurnitureComponent * comp = _componentContainer->getComponentForType(componentType);
	return comp;
}


