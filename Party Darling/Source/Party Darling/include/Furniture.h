#pragma once

#include <MeshEntity.h>
#include <Entity.h>
#include <BulletMeshEntity.h>
#include <RoomLayout.h>
#include <RoomObject.h>
#include <vector>

class ComponentShaderBase;

class Furniture: public RoomObject {
public:

	Furniture(BulletWorld * _world, TriMesh * _mesh, ComponentShaderBase * _shader = nullptr, Anchor_t _anchor = Anchor_t::GROUND);
	~Furniture(void);
};
