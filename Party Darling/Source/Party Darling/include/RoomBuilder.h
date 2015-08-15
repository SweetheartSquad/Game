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

enum Tile_t{
	kNONE,
	kSIDE,
	kCORNER
};

enum Side_t;

class BulletWorld;
class MeshInterface;

class Room;
class RoomObject;
class Furniture;
class Person;
class Item;

class Room;

class Tile{
public:
	glm::vec2 pos;
	Tile_t type;
	bool free;

	Tile(glm::vec2 _pos, Tile_t _type);
};

class Edge{
public:
	glm::vec2 p1;
	glm::vec2 p2;
	float angle;

	Edge(glm::vec2 _p1, glm::vec2 _p2);
};

class Slot;

// Use builder pattern (parsing flat data)
class RoomBuilder{
public: 

	static Room * getRoom(std::string json, BulletWorld * _world);

	// Furniture placement
	static bool search(RoomObject * child, std::vector<RoomObject *> objects);
	static bool arrange(RoomObject * child, RoomObject * parent, Side_t side, Slot * slot);

	// Room boundaries builder functions
	static std::vector<RoomObject *> getBoundaries(BulletWorld * _world, RoomLayout_t type, glm::vec2 size);
	static RoomObject * getWall(BulletWorld * _world, float width, glm::vec2 pos, float angle);
	static std::vector<RoomObject *> getRectRoom(BulletWorld * _world, glm::vec2 size);
	static std::vector<RoomObject *> getTRoom(BulletWorld * _world, glm::vec2 size);
	static std::vector<RoomObject *> getLRoom(BulletWorld * _world, glm::vec2 size);
	static std::vector<RoomObject *> box(BulletWorld * _world, glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top = true, bool bottom = true);

	static std::vector<Tile *> getTiles(RoomLayout_t _type, glm::vec2 _size);

	// Create random room objects, including specified objects
	static std::vector<RoomObject *> getRoomObjects(Json::Value _json, BulletWorld * _world);
	static std::vector<Person *> getCharacters(Json::Value _json, BulletWorld * _world);
	static std::vector<Furniture *> getFurniture(Json::Value _json, BulletWorld * _world);
	static std::vector<Item *> getItems(Json::Value _json, BulletWorld * _world);
	
	static Person * RoomBuilder::readCharacter(Json::Value _json, BulletWorld * _world);
	static Furniture * RoomBuilder::readFurniture(Json::Value _json, BulletWorld * _world);
	static Item * RoomBuilder::readItem(Json::Value _json, BulletWorld * _world);
	static RoomObject * readRoomObject(Json::Value _roomObject, RoomObject_t _type, BulletWorld * _world);
};
