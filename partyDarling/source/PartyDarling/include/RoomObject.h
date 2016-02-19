#pragma once

#include <BulletMeshEntity.h>
#include <map>

enum Anchor_t{
	GROUND,
	WALL,
	CIELING
};

enum class PD_Side {
	kFRONT	= 0,
	kBACK	= 1,
	kRIGHT  = 2,
	kTOP	= 3,
	kLEFT	= 4,
	kBOTTOM	= 5,
	kNONE	= 6
}; 

struct PD_ParentDef{
	std::string parent;
	std::vector<PD_Side> sides;

	friend bool operator==(const PD_ParentDef& lhs, const PD_ParentDef& rhs) {
		return lhs.parent == rhs.parent
			&& lhs.sides == rhs.sides;
	}

	friend bool operator!=(const PD_ParentDef& lhs, const PD_ParentDef& rhs) {
		return !(lhs == rhs);
	}
};

class PD_Slot;

class RoomObject: public BulletMeshEntity {
public:
	std::string type;
	std::vector<PD_ParentDef> parentTypes;

	Anchor_t anchor;
	sweet::Box boundingBox;
	// map of vectors of available slots per side of an object (i guess this is here in case we split a side? e.g. place a small thing in the center of a side, you get two new slots on either side of it)
	std::map<PD_Side, PD_Slot *> emptySlots;

	RoomObject * parent;

	RoomObject(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);
};
