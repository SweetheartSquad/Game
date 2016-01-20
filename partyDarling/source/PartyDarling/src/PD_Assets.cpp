#pragma once

#include <PD_Assets.h>
#include <PD_Item.h>

#include <scenario/Scenario.h>


//texture = new Texture("assets/SCENARIO_EDITOR_STUFF/components/"+texJson.get("name", "DEFAULT").asString(), true, false, texJson.get("useMipmaps", false).asBool());

// character
CharacterComponentDefinition::CharacterComponentDefinition(Json::Value _json) :
	in(0,0),
	texture("")
{ 
	Json::Value componentJson, childComponentsJson;
	// parse external component file
	if(_json.isMember("src")){
		Json::Reader reader;
		bool parsingSuccesful = reader.parse(sweet::FileUtils::readFile("assets/textures/" + _json.get("src", "NO_SRC").asString()), componentJson);
		if(!parsingSuccesful){
			Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
		}

		// we don't care about the textures array, so just grab the joints
		componentJson = componentJson["joints"];
		// there can't be components in both the skeletal definition and the component definition
		// because what would that even do
		if(componentJson["components"].size() > 0 && _json["components"].size() > 0){
			throw "hey you can't do that dummy";
		}else if(componentJson["components"].size() > 0){
			childComponentsJson = componentJson["components"];
		}else{
			childComponentsJson = _json["components"];
		}
	}else{
		componentJson = _json;
		childComponentsJson = componentJson["components"];
	}

	

	in = glm::vec2(componentJson["in"].get(Json::Value::ArrayIndex(0), 0.5f).asFloat(), componentJson["in"].get(Json::Value::ArrayIndex(1), 0.5f).asFloat());

	for(Json::ArrayIndex i = 0; i < componentJson["out"].size(); ++i){
		Json::Value jointPercentagesJson = componentJson["out"][i];
		// other joints are out joints
		out.push_back(glm::vec2(componentJson["out"][i].get(Json::Value::ArrayIndex(0), 0.5f).asFloat(), componentJson["out"][i].get(Json::Value::ArrayIndex(1), 0.5f).asFloat()));
	}
	// get texture reference
	texture = componentJson.get("texture", "NO_TEXTURE").asString();

	// parse child components from component definition
	for(Json::ArrayIndex i = 0; i < childComponentsJson.size(); ++i){
		components.push_back(CharacterComponentDefinition(childComponentsJson[i]));
	}
}

AssetCharacter::AssetCharacter(Json::Value _json, Scenario * const _scenario) :
	Asset(_json, _scenario),
	name(_json.get("name", "NO_NAME").asString()),
	root(_json["components"][0]),
	defaultState(_json.get("defaultState", "NO_STATE").asString())
{
	Json::Value statesJson = _json["states"];
	for(Json::ArrayIndex i = 0; i < statesJson.size(); ++i){
		states.insert(std::make_pair(statesJson[i].get("id", "NO_ID").asString(), PersonState(statesJson[i])));
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

PersonRenderer * AssetCharacter::getCharacter(BulletWorld * _world, Shader * _shader){
	PersonRenderer * res = new PersonRenderer(_world, this, _shader);
	res->setShader(_shader, true);
	return res;
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
	Asset(_json, _scenario),
	name(_json.get("name", "NO_NAME").asString()),
	description(_json.get("description", "NO_DESCRIPTION").asString()),
	locked(_json.get("locked", false).asBool())
{
	std::string sizeString = _json.get("size", "MEDIUM").asString();
	if(sizeString == "SMALL"){
		size = kSMALL;
	}else if(sizeString == "MEDIUM"){
		size = kMEDIUM;
	}else{
		size = kLARGE;
	}
	
	for(auto c : _json["characters"]){
		characters.push_back(c.asString());
	}
	for(auto i : _json["items"]){
		items.push_back(i.asString());
	}
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

AssetCharacter * AssetRoom::getCharacter(unsigned long int _index){
	return dynamic_cast<AssetCharacter *>(scenario->getAsset("character", characters.at(_index)));
}
std::vector<AssetCharacter *> AssetRoom::getCharacters(){
	std::vector<AssetCharacter *> res;
	for(unsigned long int c = 0; c < characters.size(); ++c){
		res.push_back(getCharacter(c));
	}

	return res;
}