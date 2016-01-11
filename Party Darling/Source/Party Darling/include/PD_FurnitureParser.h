#pragma once

#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureComponent.h>

class PD_FurnitureParser {
public:

	static std::vector<PD_FurnitureDefinition *> parseFurnitureDefinitions();
	static std::vector<PD_FurnitureComponent *> parseFurnitureComponents();

};
