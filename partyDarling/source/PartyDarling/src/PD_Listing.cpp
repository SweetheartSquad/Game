#pragma once

#include <PD_Listing.h>

std::map<Scenario *, PD_Listing *> PD_Listing::listings;
std::map<std::string, PD_Listing *> PD_Listing::listingsById;

PD_Listing::PD_Listing(Scenario * _scenario){
	this->scenario = _scenario;
	listings[_scenario] = this;
	listingsById[_scenario->id] = this;
}

PD_Listing::~PD_Listing(){
	listings.erase(listings.find(scenario));
	listingsById.erase(listingsById.find(scenario->id));
}

void PD_Listing::addCharacter(PD_Character * _person){
	assert(characters.count(_person->definition->id) == 0);
	characters[_person->definition->id] = _person;
}
void PD_Listing::addItem(PD_Item * _item){
	assert(items.count(_item->definition->id) == 0);
	items[_item->definition->id] = _item;
}
void PD_Listing::addRoom(Room * _room){
	assert(rooms.count(_room->definition->id) == 0);
	rooms[_room->definition->id] = _room;
}

void PD_Listing::removeListing(Scenario * _scenario){
	delete listings[_scenario];
}