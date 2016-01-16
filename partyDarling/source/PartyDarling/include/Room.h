#pragma once

#include <Entity.h>
#include <BulletMeshEntity.h>
#include <MeshInterface.h>
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

class Room: public BulletMeshEntity {
public:
	std::vector<RoomObject *> components;
	Sprite * tilemapSprite;
	
	std::vector<RoomObject *> objects;

	Room(BulletWorld * _world, MeshInterface * _mesh = new MeshInterface(GL_QUADS, GL_STATIC_DRAW));
	~Room(void);

	std::vector<RoomObject *> getAllComponents();

	void addComponent(RoomObject *);
	void setShader(Shader * _shader, bool _default);

	void translatePhysical(glm::vec3 _v, bool _relative = false);
};
