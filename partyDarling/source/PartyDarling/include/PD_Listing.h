#pragma once

#include <scenario/Scenario.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <PD_Item.h>
//#include <PD_Furniture.h>

// listing for an individual scenario
class PD_Listing{
public:
	std::map<std::string, Person *> characters;
	std::map<std::string, PD_Item *> items;
	//std::map<std::string, PD_Furniture *> furniture;

	static std::map<Scenario *, PD_Listing *> listings;

	PD_Listing(Scenario * _scenario);
	~PD_Listing();

	void addCharacter(Person * _person);
	void addItem(PD_Item * _item);
};