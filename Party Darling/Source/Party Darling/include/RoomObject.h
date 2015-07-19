#pragma once

#include <BulletMeshEntity.h>

enum Anchor_t{
	GROUND,
	WALL
};

class ComponentShaderBase;

class RoomObject: public BulletMeshEntity {
public:
	
	Anchor_t anchor;

	RoomObject(BulletWorld * _world, MeshInterface * _mesh, ComponentShaderBase * _shader = nullptr, bool _convex = false, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);
};
