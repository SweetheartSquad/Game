#pragma once

#include <MeshInterface.h>
#include <RoomObject.h>
#include <vector>
#include <json\json.h>

#define ROOM_HEIGHT 20
#define ROOM_UNIT 20
#define ROOM_TILE 4

class Room;
class Furniture;
class Character;
class Item;

enum RoomLayout_t{
	RECT,
	T,
	L
};

enum RoomObject_t{
	CHARACTER,
	FURNITURE,
	ITEM
};

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
	static std::vector<RoomObject *> getRoomObjects(Json::Value json, BulletWorld * _world);
	static std::vector<Character *> getCharacters(Json::Value json, BulletWorld * _world);
	static std::vector<Furniture *> getFurniture(Json::Value json, BulletWorld * _world);
	static std::vector<Item *> getItems(Json::Value json, BulletWorld * _world);
	
	static Character * RoomLayout::readCharacter(Json::Value _json, BulletWorld * _world);
	static Furniture * RoomLayout::readFurniture(Json::Value _json, BulletWorld * _world);
	static Item * RoomLayout::readItem(Json::Value _json, BulletWorld * _world);
	static RoomObject * readRoomObject(Json::Value _roomObject, RoomObject_t _type, BulletWorld * _world);
};
