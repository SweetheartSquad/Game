#pragma once

#include <MeshEntity.h>
#include <Entity.h>
#include <BulletMeshEntity.h>
#include <RoomLayout.h>
#include <vector>

class ComponentShaderBase;

class Room: public Entity {
public:

	RoomLayout * roomLayout;

	//std::vector<MeshEntity *> mesh;
	std::vector<BulletMeshEntity *> boundaries;

	std::vector<int> furnitureSlots;
	std::vector<int> characterSlots;

	Room(BulletWorld * _world, ComponentShaderBase * _shader, RoomLayout_t _type, glm::vec2 _size, Texture * _wallTexture);
	~Room(void);
};
