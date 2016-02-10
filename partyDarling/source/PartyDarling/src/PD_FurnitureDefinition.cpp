#pragma once

#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureComponentDefinition.h>
#include <RoomObject.h>

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
	padding(_jsonDef.get("padding", 0.f).asFloat()),
	detailedCollider(_jsonDef.get("detailedCollider", true).asBool()),
	sides(PD_FurnitureSides(_jsonDef["sides"]))
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
}
