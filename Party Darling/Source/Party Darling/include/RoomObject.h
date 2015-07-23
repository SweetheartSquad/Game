#pragma once

#include <BulletMeshEntity.h>
#include <map>

enum Anchor_t{
	GROUND,
	WALL
};

enum Side_t{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	FRONT,
	BACK
};

class RoomObject: public BulletMeshEntity {
public:
	
	Anchor_t anchor;

	std::map<Side_t, RoomObject *> childSlots;

	RoomObject(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);
};
