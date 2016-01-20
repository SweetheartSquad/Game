#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>
#include <MeshInterface.h>

class PD_FurnitureComponentContainer;

struct PD_FurnitureConnector{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class PD_FurnitureComponent {
public:
	int id;
	std::string type;
	std::string src;
	const TriMesh * const mesh;

	std::map<std::vector<std::string>, std::vector<PD_FurnitureConnector>> connectors; 


	explicit PD_FurnitureComponent(Json::Value _jsonDef);
};