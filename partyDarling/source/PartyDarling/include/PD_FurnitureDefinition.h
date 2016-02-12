#pragma once

#include <string>
#include <json/json.h>
#include <RoomObject.h>

class PD_FurnitureComponentDefinition;

class PD_FurnitureSides{
public:
	PD_Side front;
	PD_Side back;
	PD_Side left;
	PD_Side right;
	PD_Side top;

	explicit PD_FurnitureSides(Json::Value _jsonDef);

	static PD_Side fromString(std::string _side);
};

class PD_FurnitureDefinition {
public:

	std::string type;
	float mass;
	float padding;
	bool detailedCollider;
	std::vector<PD_FurnitureComponentDefinition *> components;
	PD_FurnitureSides sides;
	std::vector<PD_ParentDef> parents;
	std::vector<std::string> roomTypes;

	explicit PD_FurnitureDefinition(Json::Value _jsonDef);
};
