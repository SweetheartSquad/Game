#pragma once

#include <BulletMeshEntity.h>
#include <map>
#include <Light.h>

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
	MeshEntity * boundingBoxMesh;

	// angle for billboarding
	float angle;
	bool billboarded;
	void billboard(glm::vec3 _playerPos);

	std::string type;
	bool parentDependent;
	std::vector<PD_ParentDef> parentTypes;
	int parentMax; // max allowed on parent

	Anchor_t anchor;
	float padding;
	sweet::Box boundingBox;
	// map of vectors of available slots per side of an object (i guess this is here in case we split a side? e.g. place a small thing in the center of a side, you get two new slots on either side of it)
	std::map<PD_Side, PD_Slot *> emptySlots;

	RoomObject * parent;
	std::vector<RoomObject *> children; // all children (for parentMax)

	glm::vec3 originalPos; // Due to the y-axis alignment thing

	std::vector<Light *> lights;

	RoomObject(BulletWorld * _world, MeshInterface * _mesh, Shader * _shader, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);

	void resetObject();
	// used when centering furniture, so that when a room object is repositioned, its "children" follow
	void moveChildren(glm::vec3 _translation, bool _relative = true);
};
