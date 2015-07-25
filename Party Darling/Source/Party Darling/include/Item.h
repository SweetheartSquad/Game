#pragma once

#include <RoomObject.h>

class Item: public RoomObject {
public:

	Item(BulletWorld * _world, MeshInterface * _mesh);
	~Item();
};
