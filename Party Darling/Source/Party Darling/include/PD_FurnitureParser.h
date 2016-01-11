#pragma once

#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureComponent.h>

class PD_FurnitureComponentContainer {
public:
	
	std::map<std::string, std::vector<PD_FurnitureComponent *>> componentsMap;
	PD_FurnitureComponent * getComponentForType(std::string _type);
};

class PD_FurnitureParser {
public:

	static std::vector<PD_FurnitureDefinition *> * parseFurnitureDefinitions();
	static PD_FurnitureComponentContainer * parseFurnitureComponents();
};
