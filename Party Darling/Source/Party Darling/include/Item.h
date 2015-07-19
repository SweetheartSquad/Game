#pragma once

#include <MeshEntity.h>
#include <Entity.h>
#include <BulletMeshEntity.h>
#include <RoomLayout.h>
#include <RoomObject.h>
#include <vector>

class ComponentShaderBase;

class Item: public RoomObject {
public:

	Item(BulletWorld * _world, TriMesh * _mesh, ComponentShaderBase * _shader = nullptr);
	~Item(void);
};
