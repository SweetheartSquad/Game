#pragma once

#include <Entity.h>
#include <BulletWorld.h>
#include <vector>

enum Room_t{
	BEDROOM,
	LIVINGROOM,
	KITCHEN,
	BATHROOM,
	OFFICE,
	OTHER
};

enum RoomLayout_t;

class BulletMeshEntity;
class Shader;
class Texture;
class Sprite;

class RoomObject;

class Room: public Entity {
public:
	std::vector<RoomObject *> components;
	Sprite * tilemapSprite;

	std::vector<RoomObject *> boundaries;
	
	std::vector<RoomObject *> objects;

	Room(BulletWorld * _world, RoomLayout_t _type, glm::vec2 _size, Texture * _wallTexture = nullptr);
	~Room(void);

	void addComponent(RoomObject *);
	void setShader(Shader * _shader, bool _default);

	void translatePhysical(glm::vec3 _v, bool _relative = false);
};
