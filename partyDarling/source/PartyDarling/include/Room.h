#pragma once

#include <Entity.h>
#include <BulletMeshEntity.h>
#include <MeshInterface.h>
#include <BulletWorld.h>
#include <vector>

#include <PD_Door.h>

#define ROOM_SIZE_MAX 12

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
class AssetRoom;

class Room: public BulletMeshEntity {
public:
	AssetRoom * const definition;
	bool locked;
	
	enum Visibility{
		kHIDDEN, // the player doesn't know this room exists
		kSEEN, // the player has seen a door to this room, but hasn't been inside
		kENTERED // the player has been in this room
	} visibility;

	PD_TilemapGenerator * tilemap;
	std::vector<RoomObject *> components;
	Sprite * tilemapSprite;
	
	BulletMeshEntity * floor;
	BulletMeshEntity * ceiling;
	std::map<PD_Door::Door_t, RoomObject *> doors;

	Room(BulletWorld * _world, Shader * _shader, AssetRoom * const _definition);
	~Room(void);

	void addComponent(RoomObject * _obj);
	void removeComponent(RoomObject * _obj);
	virtual void setShader(Shader * _shader, bool _default) override;

	void translatePhysical(glm::vec3 _v, bool _relative = false);

	glm::vec3 getCenter() const;


	// removes all of the rigid bodies associated with the room from the world
	void removePhysics();
	// adds all of the rigid bodies associatedw with the room to the world
	void addPhysics();
};
