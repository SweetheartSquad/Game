#pragma once

#include <MeshInterface.h>
#include <vector>

#define ROOM_HEIGHT 20
#define ROOM_UNIT 20

enum RoomLayout_t{
	RECT,
	T,
	L
};

class RoomLayout{
public: 
	unsigned int id; // reference for layout specifications?

	std::vector<MeshInterface *> walls;
	
	std::vector<int> furnitureSlots;
	std::vector<int> characterSlots;

	RoomLayout(void);
	~RoomLayout(void);

	static MeshInterface * getWalls(RoomLayout_t type, glm::vec2 size);

	static QuadMesh * box(QuadMesh * m, glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top = true, bool bottom = true);
	static MeshInterface * getRectRoom(glm::vec2 size);
	static MeshInterface * getTRoom(glm::vec2 size);
	static MeshInterface * getLRoom(glm::vec2 size);
};
