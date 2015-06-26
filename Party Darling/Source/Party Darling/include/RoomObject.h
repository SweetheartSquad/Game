#pragma once

#include <MeshEntity.h>
#include <Entity.h>
#include <BulletMeshEntity.h>
#include <RoomLayout.h>
#include <vector>

enum Anchor_t{
	GROUND,
	WALL
};

class ComponentShaderBase;

class RoomObject: public BulletMeshEntity {
public:
	
	Anchor_t anchor;

	RoomObject(BulletWorld * _world, TriMesh * _mesh, ComponentShaderBase * _shader = nullptr, bool _convex = false, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);
};
