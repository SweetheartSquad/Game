#pragma once

#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureComponent.h>

class PD_FurnitureComponentContainer {
public:
	PD_FurnitureComponentContainer(std::string _src);
	std::map<std::string, std::vector<PD_FurnitureComponent *>> componentsMap;
	PD_FurnitureComponent * getComponentForType(std::string _type);
};