#pragma once

#include <scenario/Asset.h>

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
	// the item's in-game texture
	Texture * texture;

	// if an item is collectable, interacting with it with cause it to be picked up
	// if an item is not collectable, interacting with it will trigger its interaction event
	bool collectable;

	// if pixel perfect interaction is enabled, an additional check will be made after the ray intersection
	// which will only succeed if the hovered pixel is not transparent
	bool pixelPerfectInteraction;

	// effects are stored as Events
	// in order to trigger the effects, they will be copied and added to an EventManager
	std::vector<sweet::Event> effects;
	

	// substitute for public constructor (we can't take the address of the constructor,
	// so we have a static function which simply returns a new instance of the class instead)
	static AssetItem * create(Json::Value _json, Scenario * const _scenario);
	~AssetItem();
	
	virtual void load() override;
	virtual void unload() override;
};



class AssetCharacter : public Asset{
private:
	// constructor is private; use create instead if you need to instantiate directly
	AssetCharacter(Json::Value _json, Scenario * const _scenario);
public:

	// substitute for public constructor (we can't take the address of the constructor,
	// so we have a static function which simply returns a new instance of the class instead)
	static AssetCharacter * create(Json::Value _json, Scenario * const);
	~AssetCharacter();
	
	virtual void load() override;
	virtual void unload() override;
};





class AssetRoom : public Asset{
private:
	// constructor is private; use create instead if you need to instantiate directly
	AssetRoom(Json::Value _json, Scenario * const _scenario);
public:

	// substitute for public constructor (we can't take the address of the constructor,
	// so we have a static function which simply returns a new instance of the class instead)
	static AssetRoom * create(Json::Value _json, Scenario * const);
	~AssetRoom();
	
	virtual void load() override;
	virtual void unload() override;
};