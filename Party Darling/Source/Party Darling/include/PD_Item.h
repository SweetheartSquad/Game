#pragma once

#include <BulletMeshEntity.h>

class Texture;
class Shader;
class BulletWorld;

class PD_Item : public BulletMeshEntity{
public:
	// the item's in-game name
	std::string name;
	// the item's in-game description
	std::string description;

	// if an item is collectable, interacting with it with cause it to be picked up
	// if an item is not collectable, interacting with it will trigger its interaction event
	bool collectable;

	// if pixel perfect interaction is enabled, an additional check will be made after the ray intersection
	// which will only succeed if the hovered pixel is not transparent
	bool pixelPerfectInteraction;

	PD_Item(std::string _name, std::string _description, BulletWorld * _world, Texture * _texture, Shader * _shader, bool _collectable, bool _pixelPerfectInteraction);

	// called when the player clicks on an item in the world
	// _position, the world position of the intersection point, is used to potentially filter interactions
	bool interact(glm::vec3 _position);

	// given a _position in world space, checks for pixel perfect collision
	// process: world space -> object space -> UV coordinates -> 
	bool checkPixelPerfectCollision(glm::vec3 _position);


	// sets the collider as the bounding box
	// and creates a static rigid body
	void addToWorld();
};