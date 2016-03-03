#pragma once

#include <RoomObject.h>

class PD_PropDefinition : public Node{
public:
	MeshInterface * mesh;
	float mass;

	float padding;

	bool twist;
	bool flare;
	bool parentDependent;
	std::vector<PD_ParentDef> parents;
	std::vector<std::string> roomTypes;

	explicit PD_PropDefinition(Json::Value _jsonDef);
	~PD_PropDefinition();
};
