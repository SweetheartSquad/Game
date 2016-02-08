#pragma once

#include <BulletMeshEntity.h>
#include <map>

enum Anchor_t{
	GROUND,
	WALL,
	CIELING
};

enum class PD_Side {
	kFRONT,
	kBACK,
	kBOTTOM,
	kTOP,
	kLEFT,
	kRIGHT,
	kNONE
}; 

class Slot{
public:
	float loc;
	float length;
	PD_Side childSide; // The side the child can be aligned with this side

	Slot(PD_Side _childSide, float loc, float length);
};

class RoomObject: public BulletMeshEntity {
public:
	std::string type;
	std::vector<std::string> parentTypes;

	Anchor_t anchor;
	sweet::Box boundingBox;
	// map of vectors of available slots per side of an object (i guess this is here in case we split a side? e.g. place a small thing in the center of a side, you get two new slots on either side of it)
	std::map<PD_Side, std::vector<Slot *>> emptySlots;

	RoomObject * parent;

	RoomObject(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);
};
