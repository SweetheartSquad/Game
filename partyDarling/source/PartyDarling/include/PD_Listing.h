#pragma once

#include <scenario/Scenario.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <PD_Item.h>
#include <Room.h>
//#include <PD_Furniture.h>

// listing for an individual scenario
class PD_Listing{
public:
	std::map<std::string, PD_Character *> characters;
	std::map<std::string, PD_Item *> items;
	std::map<std::string, Room *> rooms;
	Scenario * scenario;
	//std::map<std::string, PD_Furniture *> furniture;

	static std::map<Scenario *, PD_Listing *> listings;
	static std::map<std::string, PD_Listing *> listingsById;

	PD_Listing(Scenario * _scenario);
	~PD_Listing();

	void addCharacter(PD_Character * _person);
	void addItem(PD_Item * _item);
	void addRoom(Room * _room);

	static void removeListing(Scenario * _scenario);
};
