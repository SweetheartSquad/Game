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
class PD_TilemapGenerator;

class Room: public BulletMeshEntity {
public:
	std::string id;
	PD_TilemapGenerator * tilemap;
	std::vector<RoomObject *> components;
	Sprite * tilemapSprite;
	
	BulletMeshEntity * floor;
	BulletMeshEntity * ceiling;
	RoomObject * door;

	Room(BulletWorld * _world, Shader * _shader);
	~Room(void);

	void addComponent(RoomObject * _obj);
	void removeComponent(RoomObject * _obj);
	virtual void setShader(Shader * _shader, bool _default) override;

	void translatePhysical(glm::vec3 _v, bool _relative = false);

	glm::vec3 getCenter() const;
};
