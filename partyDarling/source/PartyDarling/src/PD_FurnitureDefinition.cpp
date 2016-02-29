#pragma once

#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureComponentDefinition.h>
#include <RoomObject.h>
#include <Texture.h>

PD_FurnitureSides::PD_FurnitureSides(Json::Value _jsonDef) {
	left	= fromString(_jsonDef.get("LEFT", "NONE").asString());
	right	= fromString(_jsonDef.get("RIGHT", "NONE").asString());
	top		= fromString(_jsonDef.get("TOP", "NONE").asString());
	front	= fromString(_jsonDef.get("FRONT", "NONE").asString());
	back	= fromString(_jsonDef.get("BACK", "NONE").asString());
}

PD_Side PD_FurnitureSides::fromString(std::string _side) {
	if(_side == "FRONT")	return PD_Side::kFRONT;
	if(_side == "BACK")		return PD_Side::kBACK;
	if(_side == "LEFT")		return PD_Side::kLEFT;
	if(_side == "RIGHT")	return PD_Side::kRIGHT;
	if(_side == "TOP")		return PD_Side::kTOP;
	if(_side == "BOTTOM")	return PD_Side::kBOTTOM;
	return PD_Side::kNONE;
}

PD_FurnitureDefinition::PD_FurnitureDefinition(Json::Value _jsonDef) :
	type(_jsonDef.get("type", "UNDEFINED").asString()),
	mass(_jsonDef.get("mass", 0.f).asFloat()),
	paddingFront(_jsonDef.get("paddingFront", 0.f).asFloat()),
	paddingBack(_jsonDef.get("paddingBack", 0.f).asFloat()),
	paddingLeft(_jsonDef.get("paddingLeft", 0.f).asFloat()),
	paddingRight(_jsonDef.get("paddingRight", 0.f).asFloat()),
	detailedCollider(_jsonDef.get("detailedCollider", true).asBool()),
	sides(PD_FurnitureSides(_jsonDef["sides"])),
	parentDependent(_jsonDef.get("parentDependent", false).asBool()),
	twist(_jsonDef.get("twist", true).asBool()),
	flare(_jsonDef.get("flare", true).asBool())
{
	for(auto outCompJson : _jsonDef["components"]) {
		components.push_back(new PD_FurnitureComponentDefinition(outCompJson));								
	}
	for(auto parent : _jsonDef["parents"].getMemberNames()) {
		PD_ParentDef def;
		def.parent = parent;
		if(_jsonDef["parents"][parent].size() == 0) {
			for(unsigned long int i = 0; i < 5; ++i) {
				def.sides.push_back(static_cast<PD_Side>(i));
			}
		}else {
			for(auto side : _jsonDef["parents"][parent]) {
				def.sides.push_back(PD_FurnitureSides::fromString(side.asString()));
			}
		}
		parents.push_back(def);
	}
	for(auto type : _jsonDef["roomTypes"]) {
		roomTypes.push_back(type.asString());
	}

	// get a texture for the furniture type
	unsigned long int numTextures = _jsonDef.get("numTextures", 2).asUInt();
	for(unsigned long int i = 1; i <= numTextures; ++i){
		std::stringstream ss;
		ss << "assets/textures/furniture/" << type << "_" << i << ".png";
		Texture * tex = new Texture(ss.str(), false, false, true);
		textures.push(tex);
	}
	
}

PD_FurnitureDefinition::~PD_FurnitureDefinition(){
	while(textures.size() > 0){
		textures.pop(true);
	}
}