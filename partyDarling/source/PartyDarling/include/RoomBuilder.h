#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <json\json.h>

#define ROOM_HEIGHT 1
#define DEFAULT_ROOM_LENGTH 15
#define ROOM_TILE 5

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

enum Side_t;

class BulletWorld;
class MeshInterface;
class PD_TilemapGenerator;

class Room;
class RoomObject;
class Furniture;
class Person;
class Item;

class Edge{
public:
	glm::vec2 p1;
	glm::vec2 p2;
	float angle;

	Edge(glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _normal = glm::vec2(0));
};

class Slot;

// Use builder pattern (parsing flat data)
class RoomBuilder{
public: 
	PD_TilemapGenerator * tilemap;
	unsigned int thresh;

	Json::Value json;
	BulletWorld * world;

	std::vector<RoomObject *> boundaries;
	std::map<int, std::map<int, bool>> map;

	Room * room;

	RoomBuilder(std::string _json, BulletWorld * _world);
	RoomBuilder(Json::Value _json, BulletWorld * _world);
	~RoomBuilder();

	Room * getRoom();

	// Furniture placement
	bool search(RoomObject * child, std::vector<RoomObject *> objects, Room * room);
	bool arrange(RoomObject * child, RoomObject * parent, Side_t side, Slot * slot);
	bool canPlaceObject(RoomObject * _obj, glm::vec3 _pos, glm::quat _orientation);

	// Room boundaries builder functions
	void createWalls(unsigned long int _thresh);
	void addWall(BulletWorld * _world, float width, glm::vec2 pos, float angle);

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
