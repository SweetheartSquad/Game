#pragma once

#include <RoomObject.h>

class Person: public RoomObject {
public:

	Person(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor = Anchor_t::GROUND);
	~Person(void);
};
