#pragma once

#include <PD_Assets.h>
#include <PD_Item.h>


// character
AssetCharacter::AssetCharacter(Json::Value _json, Scenario * const _scenario) :
	Asset(_json, _scenario)
{
}
AssetCharacter * AssetCharacter::create(Json::Value _json, Scenario * const _scenario){
	return new AssetCharacter(_json, _scenario);
}
AssetCharacter::~AssetCharacter(){
}

void AssetCharacter::load(){
	if(!loaded){
	}
	Asset::load();
}

void AssetCharacter::unload(){
	if(loaded){
	}
	Asset::unload();
}

// item
AssetItem::AssetItem(Json::Value _json, Scenario * const _scenario) :
	Asset(_json, _scenario),
	name(_json.get("name", "NO_NAME").asString()),
	description(_json.get("description", "NO_DESCRIPTION").asString()),
	collectable(_json.get("collectable", false).asBool()),
	pixelPerfectInteraction(_json.get("pixelPerfect", true).asBool()),
	texture(nullptr)
{
	Json::Value effectsJson = _json["effects"];
	for(Json::ArrayIndex i = 0; i < effectsJson.size(); ++i){
		effects.push_back(sweet::Event(effectsJson[i]));
	}
	Json::Value texJson = _json["texture"];
	texture = new Texture("assets/SCENARIO_EDITOR_STUFF/items/"+texJson.get("name", "DEFAULT").asString(), true, false, texJson.get("useMipmaps", false).asBool());
}
AssetItem * AssetItem::create(Json::Value _json, Scenario * const _scenario){
	return new AssetItem(_json, _scenario);
}
AssetItem::~AssetItem(){
	delete texture;
}

void AssetItem::load(){
	if(!loaded){
		texture->load();
	}
	Asset::load();
}

void AssetItem::unload(){
	if(loaded){
		texture->unload();
	}
	Asset::unload();
}

PD_Item * AssetItem::getItem(BulletWorld * _world, Shader * _shader){
	return new PD_Item(this, _world, _shader);
}

// room
AssetRoom::AssetRoom(Json::Value _json, Scenario * const _scenario) :
	Asset(_json, _scenario)
{
}
AssetRoom * AssetRoom::create(Json::Value _json, Scenario * const _scenario){
	return new AssetRoom(_json, _scenario);
}
AssetRoom::~AssetRoom(){
}

void AssetRoom::load(){
	if(!loaded){
	}
	Asset::load();
}

void AssetRoom::unload(){
	if(loaded){
	}
	Asset::unload();
}