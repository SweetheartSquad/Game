#pragma once

#include <RoomObject.h>

class PD_PropDefinition {
public:

	std::string src;
	float mass;
	float padding;
	bool twist;
	bool flare;
	std::vector<PD_ParentDef> parents;
	std::vector<std::string> roomTypes;

	explicit PD_PropDefinition(Json::Value _jsonDef);
};
