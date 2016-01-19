#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>
#include <MeshInterface.h>

class PD_FurnitureComponentContainer;

class PD_FurnitureComponent {
public:
	int id;
	std::string type;
	std::string src;
	const TriMesh * const mesh;

	std::map<std::vector<std::string>, std::vector<glm::vec3>> connectors; 


	explicit PD_FurnitureComponent(Json::Value _jsonDef);
};