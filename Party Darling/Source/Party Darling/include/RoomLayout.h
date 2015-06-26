#pragma once

#include <MeshInterface.h>
#include <RoomObject.h>
#include <vector>
#include <json\json.h>

#define ROOM_HEIGHT 20
#define ROOM_UNIT 20

class Room;
class Furniture;
class Character;
class Item;

enum RoomLayout_t{
	RECT,
	T,
	L
};

class Room;
class String;

// Use builder pattern (parsing flat data)
class RoomLayout{
public: 

	RoomLayout(String json);
	//~RoomLayout(void);
	Json::Value json;

	RoomLayout(RoomLayout_t type, glm::vec2 size);
	~RoomLayout();

	static std::vector<MeshInterface *> getWalls(RoomLayout_t type, glm::vec2 size);
	static std::vector<MeshInterface *> getRectRoom(glm::vec2 size);
	static std::vector<MeshInterface *> getTRoom(glm::vec2 size);
	static std::vector<MeshInterface *> getLRoom(glm::vec2 size);
	static std::vector<MeshInterface *> box(glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top = true, bool bottom = true);

	// Create random room objects, including specified objects
	static std::vector<RoomObject *> getRoomObjects(std::vector<Character *> _characters, std::vector<Furniture *> _furniture, std::vector<Item *> _items);
	
};
