#pragma once

#include <string>
#include <json/json.h>
#include <RoomObject.h>

#include <NumberUtils.h>

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
	
	float paddingFront;
	float paddingBack;
	float paddingLeft;
	float paddingRight;

	bool detailedCollider;
	std::vector<PD_FurnitureComponentDefinition *> components;
	PD_FurnitureSides sides;
	bool overflow; // allow xz sides (LEFT,RIGHT,FRONT,BACK) to have children longer than them
	bool parentDependent;
	std::vector<PD_ParentDef> parents;
	std::vector<std::string> roomTypes;

	sweet::ShuffleVector<Texture *> textures;
	
	bool twist;
	bool flare;

	explicit PD_FurnitureDefinition(Json::Value _jsonDef);
	~PD_FurnitureDefinition();
};
