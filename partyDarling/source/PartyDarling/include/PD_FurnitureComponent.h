#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>
#include <MeshInterface.h>

class PD_FurnitureComponentContainer;

struct PD_FurnitureConnector : public Node{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class PD_FurnitureComponent : public Node{
public:
	int id;
	std::string type;
	std::string src;
	TriMesh * const mesh;

	std::map<std::vector<std::string>, std::vector<PD_FurnitureConnector>> connectors;

	explicit PD_FurnitureComponent(Json::Value _jsonDef);
	~PD_FurnitureComponent();
};