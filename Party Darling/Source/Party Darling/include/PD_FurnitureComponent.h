#pragma once

#include <string>
#include <json/json.h>
#include <vector>
#include <MeshInterface.h>

class PD_FurnitureComponentContainer;

class PD_FurnitureComponent {
public:
	int id;
	std::string type;
	std::string src;

	std::map<std::string, glm::vec3> connectors; 

	MeshInterface * mesh;

	explicit PD_FurnitureComponent(Json::Value _jsonDef);
};