#pragma once

#include <node/Node.h>
#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureComponent.h>

class PD_FurnitureComponentContainer : public Node{
public:
	std::map<std::string, std::vector<PD_FurnitureComponent *>> componentsMap;

	PD_FurnitureComponentContainer(std::string _src);
	~PD_FurnitureComponentContainer();

	PD_FurnitureComponent * getComponentForType(std::string _type);
};