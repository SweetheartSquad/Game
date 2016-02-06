#pragma once

#include <BulletMeshEntity.h>
#include <map>

enum Anchor_t{
	GROUND,
	WALL,
	CIELING
};

enum Side_t{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	FRONT,
	BACK
};

class Slot{
	public:
	float loc;
	float length;

	Slot(float loc, float length);
};

class RoomObject: public BulletMeshEntity {
public:
	std::vector<std::string> tags;
	std::vector<std::string> parentTags;

	Anchor_t anchor;
	sweet::Box boundingBox;
	// map of vectors of available slots per side of an object
	std::map<Side_t, std::vector<Slot *>> emptySlots;

	RoomObject * parent;

	RoomObject(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);
};
