#pragma once

#include <Entity.h>
#include <BulletWorld.h>
#include <RoomLayout.h>
#include <vector>

enum Room_t{
	BEDROOM,
	LIVINGROOM,
	KITCHEN,
	BATHROOM,
	OFFICE,
	OTHER
};

class BulletMeshEntity;
class ComponentShaderBase;
class Shader;
class Texture;

class RoomObject;

class Room: public Entity {
public:

	std::vector<BulletMeshEntity *> boundaries;

	std::vector<int> furnitureSlots;
	std::vector<int> characterSlots;
	std::vector<RoomObject *> objects;

	Room(BulletWorld * _world, ComponentShaderBase * _shader, RoomLayout_t _type, glm::vec2 _size, Texture * _wallTexture = nullptr);
	~Room(void);

	void setShader(Shader * _shader, bool _default);

	void translatePhysical(glm::vec3 _v);
};
