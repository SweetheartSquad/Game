#pragma once

#include <BulletFirstPersonController.h>
class Room;
class Timeout;

class Player : public virtual BulletFirstPersonController{
public:
	float shakeIntensity;
	Timeout * shakeTimeout;

	// base player speed
	float playerSpeed;
	// sprint multiplier (cumulative with playerSpeed)
	float sprintSpeed;
	// impulse applied upwards when jumping
	float jumpSpeed;
	float mass;
	float maxSpeed;

	int defense;
	int insight;
	int strenth;
	int sass;

	Player(BulletWorld * bulletWorld);
};