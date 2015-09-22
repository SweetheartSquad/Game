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

class Slot{
	public:
	float loc;
	float length;

	Slot(float loc, float length);
};

class RoomObject: public BulletMeshEntity {
public:
	
	Anchor_t anchor;
	sweet::Box boundingBox;
	// map of vectors of available slots per side of an object
	std::map<Side_t, std::vector<Slot *>> emptySlots;
	std::vector<RoomObject *> components;

	RoomObject(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor = Anchor_t::GROUND);
	~RoomObject(void);



	void addComponent(RoomObject *);
	void setShader(Shader * _shader, bool _default) override;
	void translatePhysical(glm::vec3 _v, bool _relative = false);
};
