#pragma once

#include <PD_Assets.h>
#include <PD_Item.h>


//texture = new Texture("assets/SCENARIO_EDITOR_STUFF/components/"+texJson.get("name", "DEFAULT").asString(), true, false, texJson.get("useMipmaps", false).asBool());

// character
CharacterComponent::CharacterComponent(Json::Value _json) :
	in(_json["in"].get(Json::Value::ArrayIndex(0), 0.5f).asFloat(), _json["in"].get(Json::Value::ArrayIndex(1), 0.5f).asFloat()),
	texture(_json.get("texture", "NO_TEXTURE").asString())
{ 
	// out joint percentages
	Json::Value outJson = _json["out"];
	for(Json::ArrayIndex i = 0; i < outJson.size(); ++i){
		out.push_back(glm::vec2(outJson[i].get(Json::Value::ArrayIndex(0), 0.5f).asFloat(), outJson[i].get(Json::Value::ArrayIndex(1), 0.5f).asFloat()));
	}

	// child components
	Json::Value componentsJson = _json["components"];
	for(Json::ArrayIndex i = 0; i < componentsJson.size(); ++i){
		components.push_back(CharacterComponent(componentsJson[i]));
	}
}

AssetCharacter::AssetCharacter(Json::Value _json, Scenario * const _scenario) :
	Asset(_json, _scenario),
	name(_json.get("name", "NO_NAME").asString()),
	root(_json["components"][0])
{
	Json::Value statesJson = _json["states"];
	for(Json::ArrayIndex i = 0; i < statesJson.size(); ++i){
		states.push_back(PersonState(statesJson[i]));
	}
	Json::Value itemsJson = _json["items"];
	for(Json::ArrayIndex i = 0; i < itemsJson.size(); ++i){
		items.push_back(itemsJson[i].asString());
	}
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
	texture(_json.get("texture", "NO_TEXTURE").asString())
{
	Json::Value effectsJson = _json["effects"];
	for(Json::ArrayIndex i = 0; i < effectsJson.size(); ++i){
		effects.push_back(sweet::Event(effectsJson[i]));
	}
}
AssetItem * AssetItem::create(Json::Value _json, Scenario * const _scenario){
	return new AssetItem(_json, _scenario);
}
AssetItem::~AssetItem(){
}

void AssetItem::load(){
	if(!loaded){
	//	texture->load();
	}
	Asset::load();
}

void AssetItem::unload(){
	if(loaded){
	//	texture->unload();
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