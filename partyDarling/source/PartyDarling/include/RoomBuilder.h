#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <json\json.h>
#include <NumberUtils.h>

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

class Room;
class RoomObject;
class PD_Furniture;
class Person;
class Item;
class Shader;
class Texture;

class Edge{
public:
	glm::vec2 p1;
	glm::vec2 p2;
	float angle;

	Edge(glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _normal = glm::vec2(0));
};

class Slot;
class AssetRoom;

// Use builder pattern (parsing flat data)
class RoomBuilder{
private:
	Shader * baseShader;
	Shader * characterShader;

	// the definition for this room
	AssetRoom * const definition;
	
	sweet::ShuffleVector<unsigned long int> debugTexIdx;
	sweet::ShuffleVector<unsigned long int> wallTexIdx;
	sweet::ShuffleVector<unsigned long int> ceilTexIdx;
	sweet::ShuffleVector<unsigned long int> floorTexIdx;
public:
	unsigned long int thresh;

	BulletWorld * world;

	std::vector<RoomObject *> boundaries;

	std::vector<RoomObject *> availableParents;

	Room * room;

	RoomBuilder(AssetRoom * const _definition, BulletWorld * _world, Shader * _baseShader, Shader * _characterShader);
	~RoomBuilder();

	Room * getRoom();

	// Furniture placement
	bool search(RoomObject * child);
	bool arrange(RoomObject * child, RoomObject * parent, Side_t side, Slot * slot);
	bool canPlaceObject(RoomObject * _obj, glm::vec3 _pos, glm::quat _orientation);
	bool canBeParent(RoomObject * _obj, bool _addToList = true);

	// Room boundaries builder functions

	// Create room walls from tilemap
	void createWalls();
	void addWall(float width, glm::vec2 pos, float angle);

	// Create random room objects, including specified objects
	std::vector<RoomObject *> getRoomObjects();
	std::vector<Person *> getCharacters();
	std::vector<PD_Furniture *> getFurniture();
	std::vector<Item *> getItems();

	Texture * getFloorTex();
	Texture * getCeilTex();
	Texture * getWallTex();
	Texture * getDebugTex();
};
