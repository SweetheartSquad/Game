#pragma once

#include <string>
#include <json/json.h>
#include <vector>

class PD_FurnitureConnector {
public:	
	std::string description;
	std::string positionX;
	std::string positionY;
	std::string positionZ;

	explicit PD_FurnitureConnector(Json::Value _jsonDef);
};

class PD_FurnitureComponent {
public:
	int id;
	std::string type;
	std::string src;

	std::vector<PD_FurnitureConnector *> connectors; 

	explicit PD_FurnitureComponent(Json::Value _jsonDef);
};