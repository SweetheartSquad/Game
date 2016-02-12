#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <json\json.h>
#include <NumberUtils.h>
#include <PD_Listing.h>

#define ROOM_HEIGHT 1
#define DEFAULT_ROOM_LENGTH 15
// world space units per pixel inthe tilemap
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

class BulletWorld;
class MeshInterface;

class Room;
class RoomObject;
class PD_Furniture;
class PD_Prop;
class Person;
class Item;
class Shader;
class Texture;

class Edge{
public:
	glm::vec2 p1;
	glm::vec2 p2;
	float angle;

	float slope;
	float yIntercept;

	Edge(glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _normal = glm::vec2(0));

	bool intersects(Edge * _ray, float _scale = 1.f);
	bool isVertical();
	bool isInside(glm::vec2 _point, float _scale = 1.f);
};

class Slot;
class AssetRoom;

// Use builder pattern (parsing flat data)
class RoomBuilder{
private:
	Shader * baseShader;
	Shader * characterShader;
	Shader * emoteShader;

	// the definition for this room
	AssetRoom * const definition;
	
	static sweet::ShuffleVector<unsigned long int> debugTexIdx;
	static sweet::ShuffleVector<unsigned long int> wallTexIdx;
	static sweet::ShuffleVector<unsigned long int> ceilTexIdx;
	static sweet::ShuffleVector<unsigned long int> floorTexIdx;
	static sweet::ShuffleVector<unsigned long int> doorTexIdx;
	static bool staticInit();
	static bool staticInitialized;
public:
	unsigned long int thresh;

	BulletWorld * world;

	std::vector<RoomObject *> boundaries;

	std::vector<RoomObject *> availableParents;
	std::vector<RoomObject *> placedObjects;

	Room * room;
	glm::vec3 roomUpperBound;
	glm::vec3 roomLowerBound;

	std::vector<Edge *> edges;

	RoomBuilder(AssetRoom * const _definition, BulletWorld * _world, Shader * _baseShader, Shader * _characterShader, Shader * _emoteShader);
	~RoomBuilder();

	Room * getRoom();

	// Furniture placement
	bool search(RoomObject * child);
	bool arrange(RoomObject * child, RoomObject * parent, PD_Side side, Slot * slot);
	bool canPlaceObject(RoomObject * _obj, glm::vec3 _pos, glm::quat _orientation);
	void addObjectToLists(RoomObject * _obj);
	bool canBeParent(RoomObject * _obj);

	//transform vertices from a's space into b
	std::vector<glm::vec3> getLocalBoundingBoxVertices(std::vector<glm::vec3> _verts, glm::mat4 _mmA, glm::mat4 _mmB);
	// Room boundaries builder functions

	// Create room walls from tilemap
	void createWalls();
	void addWall(float width, glm::vec2 pos, float angle);

	// Create random room objects, including specified objects
	std::vector<RoomObject *> getRoomObjects();
	std::vector<Person *> getCharacters();
	std::vector<PD_Furniture *> getFurniture();
	std::vector<PD_Item *> getItems();
	std::vector<PD_Prop *> getProps();

	Texture * getFloorTex();
	Texture * getCeilTex();
	Texture * getWallTex();
	Texture * getDebugTex();
};
