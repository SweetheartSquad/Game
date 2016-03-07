#pragma once

#include <BulletFirstPersonController.h>
class Room;
class Timeout;
class PD_DissStats;

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

	float experience;
	int level;

	PD_DissStats * dissStats;
	bool wonLastDissBattle;
	Player(BulletWorld * bulletWorld);
	~Player();

	glm::vec3 calculateInputs(Step * _step) override;

	virtual void update(Step * _step) override;
};