#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <json\json.h>

class PD_House;
class BulletWorld;

// Use builder pattern (parsing flat data)
class PD_HouseGenerator{
public: 

	Json::Value json;
	BulletWorld * world;
	
	// _json: an object with an array of scenarios
	PD_HouseGenerator(std::string _json, BulletWorld * _world);
	~PD_HouseGenerator();

	PD_House * getHouse();

	static Json::Value bundleScenarios(Json::Value _json);

};
