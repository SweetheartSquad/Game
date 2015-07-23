#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <json\json.h>

#define ROOM_HEIGHT 5
#define DEFAULT_ROOM_LENGTH 15
#define ROOM_TILE 4

enum RoomLayout_t{
	kRECT,
	kT,
	kL
};

enum RoomObject_t{
	CHARACTER,
	FURNITURE,
	ITEM
};

class BulletWorld;
class MeshInterface;
class ComponentShaderBase;

class Room;
class RoomObject;
class Furniture;
class Person;
class Item;

class Room;

// Use builder pattern (parsing flat data)
class RoomLayout{
public: 

	static Room * getRoom(std::string json, BulletWorld * _world, ComponentShaderBase * _shader);

	Json::Value json;

	RoomLayout(RoomLayout_t type, glm::vec2 size);
	~RoomLayout();

	static std::vector<MeshInterface *> getWalls(RoomLayout_t type, glm::vec2 size);
	static std::vector<MeshInterface *> getRectRoom(glm::vec2 size);
	static std::vector<MeshInterface *> getTRoom(glm::vec2 size);
	static std::vector<MeshInterface *> getLRoom(glm::vec2 size);
	static std::vector<MeshInterface *> box(glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top = true, bool bottom = true);

	// Create random room objects, including specified objects
	static std::vector<RoomObject *> getRoomObjects(Json::Value _json, BulletWorld * _world);
	static std::vector<Person *> getCharacters(Json::Value _json, BulletWorld * _world);
	static std::vector<Furniture *> getFurniture(Json::Value _json, BulletWorld * _world);
	static std::vector<Item *> getItems(Json::Value _json, BulletWorld * _world);
	
	static Person * RoomLayout::readCharacter(Json::Value _json, BulletWorld * _world);
	static Furniture * RoomLayout::readFurniture(Json::Value _json, BulletWorld * _world);
	static Item * RoomLayout::readItem(Json::Value _json, BulletWorld * _world);
	static RoomObject * readRoomObject(Json::Value _roomObject, RoomObject_t _type, BulletWorld * _world);
};
