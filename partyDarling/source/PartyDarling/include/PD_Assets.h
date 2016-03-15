#pragma once

#include <scenario/Asset.h>
#include <PD_Character.h>

class PD_Item;
class BulletWorld;
class Shader;

/*
// Template for new asset type:
class AssetTYPE : public Asset{
private:
// constructor is private; use create instead if you need to instantiate directly
AssetTYPE(Json::Value _json, Scenario * const _scenario);
public:

// substitute for public constructor (we can't take the address of the constructor,
// so we have a static function which simply returns a new instance of the class instead)
static AssetTYPE * create(Json::Value _json, Scenario * const _scenario);
~AssetTYPE();

virtual void load() override;
virtual void unload() override;
};
*/

// an asset containing an item definition
// which can be used to instatiate new items
class AssetItem : public Asset{
private:
	// constructor is private; use create instead if you need to instantiate directly
	AssetItem(Json::Value _json, Scenario * const _scenario);
public:
	// returns a new instance of the item based on this asset definition
	PD_Item * getItem(BulletWorld * _world, Shader * _shader);

	// the item's in-game name
	std::string name;
	// the item's in-game description
	std::string description;
	// reference to the asset id for the item's in-game texture
	std::string texture;

	// if an item is collectable, interacting with it with cause it to be picked up
	// if an item is not collectable, interacting with it will trigger its interaction event
	bool collectable;

	// if pixel perfect interaction is enabled, an additional check will be made after the ray intersection
	// which will only succeed if the hovered pixel is not transparent
	bool pixelPerfectInteraction;

	// In an item is consumable then it disapears after being used
	bool consumable;

	// effects are stored as Events
	// in order to trigger the effects, they will be copied and added to an EventManager
	std::vector<sweet::Event> effects;
	// effects are stored as Events
	// in order to trigger the effects, they will be copied and added to an EventManager
	std::vector<sweet::Event> pickupEffects;

	// substitute for public constructor (we can't take the address of the constructor,
	// so we have a static function which simply returns a new instance of the class instead)
	static AssetItem * create(Json::Value _json, Scenario * const _scenario);
	~AssetItem();

	virtual void load() override;
	virtual void unload() override;
};

class CharacterComponentDefinition : public NodeContent{
public:
	glm::vec2 in;
	std::vector<glm::vec2> out;
	// reference to the asset id for the component's in-game texture
	std::string texture;
	std::vector<CharacterComponentDefinition> components;

	CharacterComponentDefinition(Json::Value _json);
};

// an asset containing a character definition
// which can be used to instatiate new characters
class AssetCharacter : public Asset{
private:
	// constructor is private; use create instead if you need to instantiate directly
	AssetCharacter(Json::Value _json, Scenario * const _scenario);
public:
	// the character's in-game name
	std::string name;
	// states
	std::map<std::string, CharacterState> states;
	// initial state upon creation
	std::string defaultState;
	// items held by character on creation
	// they are asset string ids; accessed using scenario->getAsset("item",items.at(#))
	std::vector<std::string> items;
	//
	CharacterComponentDefinition root;

	int strength;
	int sass;
	int defense;
	int insight;
	bool visible;
	std::string voice;
	bool enabled;
	// substitute for public constructor (we can't take the address of the constructor,
	// so we have a static function which simply returns a new instance of the class instead)
	static AssetCharacter * create(Json::Value _json, Scenario * const);
	~AssetCharacter();

	virtual void load() override;
	virtual void unload() override;

	std::vector<AssetItem *> getItems();
	AssetItem * getItem(unsigned long int _index);
};

class AssetRoom : public Asset{
private:
	// constructor is private; use create instead if you need to instantiate directly
	AssetRoom(Json::Value _json, Scenario * const _scenario);
public:
	// this room's type
	std::string roomType;
	// this room's in-game name (probably not visible)
	std::string name;
	// this room's in-game description (probably not visible)
	std::string description;
	// the ids of the characters who belong in this room
	std::vector<std::string> characters;
	// the ids of the items that belong in this room
	std::vector<std::string> items;
	// Triggers that occur the first time the player enters the room
	std::vector<sweet::Event> triggersOnce;
	// Triggers that occur every time the player enters the room
	std::vector<sweet::Event> triggersMulti;

	// the rooms size category
	enum Size_t{
		kSMALL,
		kMEDIUM,
		kLARGE
	} size;

	// whether entry to the room is blocked at creation
	bool locked;
	// Whether the room can have characters or not
	bool empty;

	std::vector<AssetCharacter *> getCharacters();
	AssetCharacter * getCharacter(unsigned long int _index);

	std::vector<AssetItem *> getItems();
	AssetItem * getItem(unsigned long int _index);

	// substitute for public constructor (we can't take the address of the constructor,
	// so we have a static function which simply returns a new instance of the class instead)
	static AssetRoom * create(Json::Value _json, Scenario * const);
	~AssetRoom();

	virtual void load() override;
	virtual void unload() override;
};