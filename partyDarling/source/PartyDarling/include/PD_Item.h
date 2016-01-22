#pragma once

#include <RoomObject.h>
#include <EventManager.h>

class Texture;
class Shader;
class BulletWorld;

class AssetItem;

#define ITEM_POS_Y 2

class PD_Item : public RoomObject{
public:
	const AssetItem * const definition;
	PD_Item(const AssetItem * const _definition, BulletWorld * _world, Shader * _shader, Anchor_t _anchor = GROUND);

	// if pixel perfect interaction is enabled, returns the result
	// otherwise, returns true
	bool actuallyHovered(glm::vec3 _position);

	// given a _position in world space, checks for pixel perfect collision
	// process: world space -> object space -> UV coordinates -> 
	bool checkPixelPerfectCollision(glm::vec3 _position);


	// sets the collider as the bounding box
	// and creates a static rigid body
	void addToWorld();


	// trigger all of this item's pickup effects on its scenario
	void triggerPickup();
	// trigger all of this item's effects on its scenario
	void triggerInteract();
};