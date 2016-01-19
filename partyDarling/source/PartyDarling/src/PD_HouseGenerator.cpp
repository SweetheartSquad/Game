#pragma once

#include <PD_HouseGenerator.h>
#include <RoomBuilder.h>

#include <PD_House.h>
#include <Room.h>
#include <MeshFactory.h>
#include <scenario\Asset.h>

#include <math.h>
#include <string.h>

#include <Resource.h>

#include <PD_ResourceManager.h>

#include <stb/stb_image.h>

#include <PD_TilemapGenerator.h>
#include <TextureUtils.h>
#include <NumberUtils.h>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Sprite.h>
#include <Texture.h>

#define HG_DEBUG 0

PD_HouseGenerator::PD_HouseGenerator(std::string _json, BulletWorld * _world):
	world(_world)
{
	Json::Reader reader;
	if(!reader.parse(_json, json, false))
	{
		// throw exception
	}
}

PD_HouseGenerator::~PD_HouseGenerator(){
}

PD_House * PD_HouseGenerator::getHouse(){
	PD_House * house = new PD_House();

	/*Json::Value bundles = PD_HouseGenerator::bundleScenarios(json);

	for(Json::ArrayIndex i = 0; i < json.size(); ++i){
		RoomBuilder * rb = new RoomBuilder(json[i], world, nullptr, nullptr);
		house->rooms.push_back(rb->getRoom());
	}*/

	return house;
}

Json::Value PD_HouseGenerator::bundleScenarios(Json::Value _scenarios){
	Json::Value bundles = new Json::Value();

	Json::Value rooms = new Json::Value();

	for(Json::ArrayIndex i = 0; i < _scenarios.size(); ++i){
		
	}

	return bundles;
}