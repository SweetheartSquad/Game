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

class Room;

class RoomLayout{
public: 
	//std::vector<MeshEntity *> mesh;
	//std::vector<BulletMeshEntity *> boundaries;

	//std::vector<int> furnitureSlots;
	//std::vector<int> characterSlots;

	//RoomLayout(void);
	//~RoomLayout(void);

	RoomLayout(RoomLayout_t type, glm::vec2 size);
	~RoomLayout();

	static std::vector<MeshInterface *> getWalls(RoomLayout_t type, glm::vec2 size);

	static std::vector<MeshInterface *> box(glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top = true, bool bottom = true);
	static std::vector<MeshInterface *> getRectRoom(glm::vec2 size);
	static std::vector<MeshInterface *> getTRoom(glm::vec2 size);
	static std::vector<MeshInterface *> getLRoom(glm::vec2 size);
};
